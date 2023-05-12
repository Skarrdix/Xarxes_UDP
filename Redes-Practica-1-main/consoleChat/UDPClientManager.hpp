#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <list>
#include <map>

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
    };

private:
    // ------ VARIABLES: ------
    sf::UdpSocket _socket;
    unsigned short _port;
    sf::IpAddress _ip;
    std::string mssg;
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
        DISCONNECT          // Packet to disconnect
    };

public:
    bool solvingChallenge = false;
    // ------ CONSTRUCTOR: ------
    UDPClientManager(sf::IpAddress ip) : _port(5001), _ip(ip) {}

    // ------ METHODS: ------
    Status Send(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* sendMessage);
    void Receive(sf::Packet& packet, std::string* rcvMessage);
    Status Connect();
    void Disconnect();

private:
    unsigned short GetLocalPort();
    sf::IpAddress GetIp();
    sf::UdpSocket* GetSocket();
};