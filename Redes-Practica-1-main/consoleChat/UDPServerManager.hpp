#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <list>
#include <map>
#include <chrono>
#include <thread>
#include "PacketLoss.h"

class UDPServerManager
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
        
        Client() = default;

        Client(std::string _username, sf::IpAddress _ip, unsigned short _port, int _id)
            : username(_username), ip(_ip), port(_port), id(_id) {}
    };

    struct NewConnection
    {
        sf::IpAddress ip;
        unsigned short port;
        std::string username;
        int ch_ts;
        int challenge1;
        int challenge2;
        int solution;

        NewConnection() = default;

        NewConnection(sf::IpAddress _ip, unsigned short _port, std::string _username, int _challenge1, int _challenge2, int _solution)
            : ip(_ip), port(_port), username(_username), challenge1(_challenge1), challenge2(_challenge2), solution(_solution) {}
    };
    
    struct PacketInfo // Serveix tant per paquets enviats com pels paquets de tipus ACK.
    {
        int id;
        sf::Packet packet;
        std::chrono::system_clock::time_point firstTimeSend; // ADDED THIS! // First TimeStamp
        std::chrono::system_clock::time_point timeSend; // Latest TimeStamp
        sf::IpAddress remoteIp;
        unsigned short remotePort;

        PacketInfo() = default;

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
    int _nextClientId;
    PacketLoss probLossManager;
    std::map<std::pair<sf::IpAddress, unsigned short>, NewConnection> _newConnections;
    int packetCount;

public:
    std::map<std::pair<sf::IpAddress, unsigned short>, Client> _clients; // NEW: added this

// ------ ENUM: ------
public:
    enum class Status
    {
        Done,               // The socket has sent / received the data correctly
        Error,              // An unexpected error happened
        Connected,          // The socket is connected and ready to work
        Disconnected        // The TCP socket is disconnected
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
// ------ CONSTRUCTOR: ------
    UDPServerManager(unsigned short port, sf::IpAddress ip);

// ------ METHODS: ------
    Status Send(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* sendMessage);
    Status ReSend(sf::Packet& packet, int packetId, sf::IpAddress ip, unsigned short port, std::string* sendMessage);
    void Receive(sf::Packet& packet, std::string* rcvMessage);
    Status Connect();
    Status Listen();
    void Disconnect();

public:
    std::map<int, PacketInfo> packetMap;
    std::vector<int> packetsToDelete;
    unsigned short GetPort();
    sf::IpAddress GetIp();
    sf::UdpSocket* GetSocket();
    void CheckTimeStampServer();
};