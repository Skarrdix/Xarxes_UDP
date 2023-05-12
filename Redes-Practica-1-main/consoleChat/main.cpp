#pragma once

#include <SFML\Network.hpp>
#include <iostream>
#include <thread>
#include "UDPServerManager.hpp"
#include "UDPClientManager.hpp"

void GetLineFromCin_t(std::string* mssg, UDPClientManager* client)
{
	std::string line;

	while (true)
	{
		if (!client->solvingChallenge) {
			std::getline(std::cin, line);
			mssg->assign(line);
		}
		
	}
}

void main()
{
	int server_mode;
	std::string mode_str;

	std::cout << "> Select a mode: (1) Server, (2) Client.\n" << std::endl;
	std::cout << "--------------------------------------------------\n" << std::endl;
	std::cout << "\t> Your input: ";

	std::cin >> mode_str;

	system("cls");

	server_mode = std::stoi(mode_str);

	UDPServerManager* _serverSocketManager = new UDPServerManager(5000, sf::IpAddress("127.0.0.1"));
	UDPClientManager* _clientSocketManager = new UDPClientManager(sf::IpAddress("127.0.0.1"));

	if (server_mode == 1) // Server
	{
		_serverSocketManager->Listen();
		std::cout << "> SERVER:\n" << std::endl;
		std::cout << "--------------------------------------------------\n" << std::endl;
		std::cout << "> Server mode running..." << std::endl;
		std::cout << "> Waiting for client to connect..." << std::endl;

		//std::thread listenForNewClient(&TCPSocketManager::Listen, _serverSocketManager);
		//listenForNewClient.detach();

		system("cls");

		std::cout << "> SERVER:\n" << std::endl;
		std::cout << "--------------------------------------------------\n" << std::endl;
		std::cout << "\t> Message to send to the client: ";

		sf::Packet _inServerPacket, _outPacket;
		std::string* _sendMessageServer = new std::string();
		std::string* _rcvMessageServer = new std::string();

		// Threads:
		std::thread rcv_t_server(&UDPServerManager::Receive, _serverSocketManager, _inServerPacket, _rcvMessageServer);
		rcv_t_server.detach();

		std::thread read_console_t(GetLineFromCin_t, _sendMessageServer, _clientSocketManager);
		read_console_t.detach();

		// Application loop:
		while (true)
		{
			// Logic for receiving:
			if (_rcvMessageServer->size() > 0)
			{
				system("cls");

				std::cout << "> SERVER:\n" << std::endl;
				std::cout << "--------------------------------------------------\n" << std::endl;

				if (*_rcvMessageServer == "exit" || *_rcvMessageServer == "Exit")
					std::cout << "> Client Disconnected." << std::endl;
				else
					std::cout << "> From Client: " << *_rcvMessageServer << std::endl;

				std::cout << "\n--------------------------------------------------\n" << std::endl;
				std::cout << "\t> Message to send to the client: ";
				
				_rcvMessageServer->clear();
			}

			// Logic for sending:
			if (_sendMessageServer->size() > 0)
			{
				for (auto client : _serverSocketManager->_clients)
				{
					if (_serverSocketManager->Send(_inServerPacket, client.first.first, client.first.second, _sendMessageServer) == UDPServerManager::Status::Disconnected)
					{
						_sendMessageServer->clear();
						_serverSocketManager->Disconnect();
						break;
					}
				}

				_sendMessageServer->clear();
			}
		}

		// Si s'arriba aquí, el servidor revient (es tanca).
	}
	else if (server_mode == 2) // Client
	{
		bool alreadyExists = false;
		std::string name;
		std::cout << "> CLIENT:\n" << std::endl;
		std::cout << "--------------------------------------------------\n" << std::endl;
		std::cout << "> Enter your Username: ";
		std::cin >> name;

		/*do {
			alreadyExists = false;
			std::cin >> name;
			for (int i = _clientSocketManager->_clients.size(); i > 0; i--) {
				if (_clientSocketManager->_clients[i-1].username == name) {
					system("cls");
					std::cout << "Username already exists, enter another Username: ";
					alreadyExists = true;
				}
			}
		} while (alreadyExists);

		_clientSocketManager->_clients.push_back(TCPSocketManager::Client(name));
		std::cout << "> Client mode running..." << std::endl;
		*/

		if (_clientSocketManager->Connect() != UDPClientManager::Status::Done)
		{
			std::cout << "\n! Error:" << std::endl;
			return;
		}
		
		//system("cls");

		std::cout << "> CLIENT:\n" << std::endl;
		std::cout << "--------------------------------------------------\n" << std::endl;
		std::cout << "> Successfully connected to server.\n" << std::endl;
		std::cout << "--------------------------------------------------\n" << std::endl;
		std::cout << "\t> Message to send to the server: ";

		sf::Packet _inClientPacket, _outPacket;
		std::string* _sendMessageClient = new std::string();
		std::string* _rcvMessageClient = new std::string();

		// Threads:
		std::thread rcv_t_client(&UDPClientManager::Receive, _clientSocketManager, _inClientPacket, _rcvMessageClient);
		rcv_t_client.detach();
		
		std::thread read_console_t_client(GetLineFromCin_t, _sendMessageClient, _clientSocketManager);
		read_console_t_client.detach();

		while (true)
		{
			// Logic for receiving:
			if (_rcvMessageClient->size() > 0)
			{
				if (*_rcvMessageClient == "exit" || *_rcvMessageClient == "Exit")
				{
					_clientSocketManager->Disconnect();
					break;
				}

				//system("cls");

				std::cout << "> CLIENT:\n" << std::endl;
				std::cout << "--------------------------------------------------\n" << std::endl;
				std::cout << "> From Server: " << *_rcvMessageClient << std::endl;
				std::cout << "\n--------------------------------------------------\n" << std::endl;
				std::cout << "\t> Message to send to the server: ";

				_rcvMessageClient->clear();
			}

			// Logic for sending:
			if (_sendMessageClient->size() > 0)
			{
				if (_clientSocketManager->Send(_inClientPacket, _serverSocketManager->GetIp(), _serverSocketManager->GetPort(), _sendMessageClient) == UDPClientManager::Status::Disconnected)
				{
					_sendMessageClient->clear();
					_clientSocketManager->Disconnect();
					break;
				}

				_sendMessageClient->clear();
			}
		}
	}
}