#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <list>
#include <map>

class UDPSocketManager
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
    sf::TcpListener _dispatcher; // No existeix el "sf::UdpListener"??
    unsigned short _port;
    sf::IpAddress _ip;
    sf::SocketSelector selector; // No sabem si això serà necessari en UDP.

    std::map<std::pair<sf::IpAddress, unsigned short>, Client> _clients; // NEW: added this

// ------ ENUM: ------
    enum class Status
    {
        Done,               ///< The socket has sent / received the data correctly
        Error,              ///< An unexpected error happened
        Connected,          ///< The socket is connected and ready to work
        Disconnected,   ///< The TCP socket is disconnected
    };

// ------ CONSTRUCTOR: ------
    UDPSocketManager(unsigned short port, sf::IpAddress ip) : _port(port), _ip(ip) {}

// ------ METHODS: ------
    Status Send(sf::Packet& packet, std::string* sendMessage);
    void Receive(sf::Packet& packet, std::string* rcvMessage);
    Status Connect();
    Status Listen();
    void Disconnect();
    void SocketSelectorFunctionality();
    
    unsigned short GetLocalPort();
    sf::IpAddress GetIp();
    sf::TcpSocket* GetSocket();
};