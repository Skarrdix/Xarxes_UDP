#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <list>
#include <map>

class TCPSocketManager
{
public:
    struct Client
    {
        std::string username;
        sf::Socket* socket;
        Client(std::string _username) {
            username = _username;
        }
    };

private:
// ------ VARIABLES: ------
    sf::TcpSocket _socket;
    sf::TcpListener _dispatcher;
    unsigned short _port;
    sf::IpAddress _ip;
    sf::SocketSelector selector;
    std::list<sf::TcpSocket*> clients;
    std::map<sf::TcpSocket*, std::string> clientNames;
public:
    std::vector<Client> _clients;

// ------ ENUM: ------
    enum class Status
    {
        Done,               ///< The socket has sent / received the data correctly
        Error,              ///< An unexpected error happened
        Connected,          ///< The socket is connected and ready to work
        Disconnected,   ///< The TCP socket is disconnected
    };

// ------ CONSTRUCTOR: ------
    TCPSocketManager(unsigned short port, sf::IpAddress ip) : _port(port), _ip(ip) {}

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