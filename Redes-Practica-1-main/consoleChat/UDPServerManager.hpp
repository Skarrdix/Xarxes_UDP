#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <list>
#include <map>

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
        
        Client(std::string _username)
        {
            username = _username;
        }
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

        NewConnection(sf::IpAddress _ip, unsigned short _port, std::string _username, int _challenge1, int _challenge2, int _solution)
        {
            ip = _ip;
            port = _port;
            username = _username;
            challenge1 = _challenge1;
            challenge2 = _challenge2;
            solution = _solution;
        }
    };

private:
// ------ VARIABLES: ------
    sf::UdpSocket _socket;
    unsigned short _port;
    sf::IpAddress _ip;

    std::map<std::pair<sf::IpAddress, unsigned short>, NewConnection> _newConnections;
    std::map<std::pair<sf::IpAddress, unsigned short>, Client> _clients; // NEW: added this

// ------ ENUM: ------
    enum class Status
    {
        Done,               // The socket has sent / received the data correctly
        Error,              // An unexpected error happened
        Connected,          // The socket is connected and ready to work
        Disconnected        // The TCP socket is disconnected
    };

    enum class PacketType
    {
        TRYCONNECTION,      // Packet to start a connection
        CANCONNECT,         // Packet to confirm connection
        CANNOTCONNECT,      // Packet to confirm failed connection
        CHALLENGE,          // Packet to send challenge question and challenge answer
        MESSAGE,            // Packet to send a message to the global chat
        DISCONNECT          // Packet to disconnect
    };

public:
// ------ CONSTRUCTOR: ------
    UDPServerManager(unsigned short port, sf::IpAddress ip) : _port(port), _ip(ip) {}

// ------ METHODS: ------
    Status Send(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* sendMessage);
    void Receive(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* rcvMessage);
    Status Connect();
    Status Listen();
    void Disconnect();

private:
    unsigned short GetLocalPort();
    sf::IpAddress GetIp();
    sf::UdpSocket* GetSocket();
};