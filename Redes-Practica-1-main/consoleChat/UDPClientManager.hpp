#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <list>
#include <map>
#include "PacketLoss.h"
#include <chrono>
#include <thread>

class UDPClientManager
{
public:
    struct Client // NEW: changed Client structure
    {
        // NEW: removed "socket"
        sf::IpAddress ip; // NEW: added this
        unsigned short port; // NEW: added this
        std::string username;
        unsigned int id; // NEW: added this
        int ts; // NEW: added this

        Client(std::string _username) {
            username = _username;
        }
        Client(std::string _username, sf::IpAddress _ip, unsigned short _port, int _id)
            : username(_username), ip(_ip), port(_port), id(_id) {}
    };

    struct PacketInfo // Serveix tant per paquets enviats com pels paquets de tipus ACK.
    {
        int id;
        sf::Packet packet;
        std::chrono::system_clock::time_point firstTimeSend; // ADDED THIS! // First TimeStamp
        std::chrono::system_clock::time_point timeSend; // Latest TimeStamp
        sf::IpAddress remoteIp;
        unsigned short remotePort;

        PacketInfo(int _id, sf::Packet _packet, std::chrono::system_clock::time_point _firstTimeSend, std::chrono::system_clock::time_point _timeSend, sf::IpAddress _remoteIp, unsigned short _remotePort)
            : id(_id), packet(_packet), firstTimeSend(_firstTimeSend), timeSend(_timeSend), remoteIp(_remoteIp), remotePort(_remotePort) {}

        PacketInfo(int _id, sf::Packet _packet, std::chrono::system_clock::time_point _timeSend, sf::IpAddress _remoteIp, unsigned short _remotePort)
            : id(_id), packet(_packet), timeSend(_timeSend), remoteIp(_remoteIp), remotePort(_remotePort) {}
    };

private:
    // ------ VARIABLES: ------
    sf::UdpSocket _socket;
    unsigned short _port;
    sf::IpAddress _ip;
    std::string mssg;
    std::map<std::pair<sf::IpAddress, unsigned short>, Client> _clients; // NEW: added this
    PacketLoss probLossManager;
// ------ ENUM: ------
public:
    enum class Status
    {
        Done,               // The socket has sent / received the data correctly
        Error,              // An unexpected error happened
        Connected,          // The socket is connected and ready to work
        Disconnected        // The TCP socket is disconnected
    };
    struct PacketInfo
    {
        int id;
        sf::Packet pakcet;
        std::chrono::duration<float, std::milli> timeSend;
    };
private:
    enum class PacketType
    {
        TRYCONNECTION,      // Packet to start a connection
        CANCONNECT,         // Packet to confirm connection
        CANNOTCONNECT,      // Packet to confirm failed connection
        CHALLENGE,          // Packet to send challenge question and challenge answer
        CHALLENGEFAILED,    // Captcha failed
        RETRYCHALLENGE,     // Retry challenge
        MESSAGE,            // Packet to send a message to the global chat
        ACK,
        DISCONNECT          // Packet to disconnect
    };

public:
    bool solvingChallenge = false;
    // ------ CONSTRUCTOR: ------
    UDPClientManager(unsigned short port, sf::IpAddress ip);

    // ------ METHODS: ------
    Status Send(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* sendMessage);
    Status ReSend(sf::Packet& packet, int packetId, sf::IpAddress ip, unsigned short port, std::string* sendMessage);
    void Receive(sf::Packet& packet, std::string* rcvMessage);
    Status Connect();
    void Disconnect();

public:
    std::map<int, PacketInfo> packetMap;
    std::vector<PacketInfo> packetArray;
    std::vector<int> packetsToDelete;
    unsigned short GetLocalPort();
    sf::IpAddress GetIp();
    sf::UdpSocket* GetSocket();
    void CheckTimeStamp();
};