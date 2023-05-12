#include "UDPServerManager.hpp"

UDPServerManager::Status UDPServerManager::Send(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* sendMessage)
{
	// Fill packet with data:
	packet << *sendMessage;

	// Send the data to the socket:
	sf::Socket::Status status = _socket.send(packet, ip, port);

	packet.clear();

	Status tempStatus;

	switch (status)
	{
		case sf::Socket::Done:
			tempStatus = Status::Done;
			break;

		case sf::Socket::NotReady:
			std::cout << "Error: NotReady.";
			tempStatus = Status::Error; // Since we do not know how sf:Socket::Status works, we return Error directly.
			break;

		case sf::Socket::Partial:
			std::cout << "Error: Partial.";
			tempStatus = Status::Error; // Since we do not know how sf:Socket::Status works, we return Error directly.
			break;

		case sf::Socket::Disconnected: // Per com hem fet el codi, retornar Status::Disconnected fa que el servidor es tanqui.
									   // per això ho canviem per Status::Error.
			std::cout << "Error: cliente desconectado.";
			tempStatus = Status::Error;
			break;

		case sf::Socket::Error:
			std::cout << "Error: al enviar paquete.";
			tempStatus = Status::Error;
			break;

		default:
			std::cout << "Error: default";
			tempStatus = Status::Error; // Since we do not know how sf:Socket::Status works, we return Error directly.
			break;
	}

	// Check if the message is "exit" or "Exit":
	if (*sendMessage == "exit" || *sendMessage == "Exit")
		return Status::Disconnected;

	// Return:
	return tempStatus;
}

void UDPServerManager::Receive(sf::Packet& packet, std::string* rcvMessage)  // No ho podem passar per referència,
																																// el valor de rcvMessage no s'aplica fora de la funció.
{
	sf::IpAddress remoteIp;
	unsigned short remotePort;

	std::string mssg_temp;

	while (true)
	{
		std::cout << "La pregunta que jo em faig" << std::endl;

		sf::Socket::Status status = _socket.receive(packet, remoteIp, remotePort);

		if (status == sf::Socket::Done)
		{
			int type;
			packet >> type;

			// Aquí gestionarem tots els diferents casos possibles de missatge (PacketType):
			switch ((PacketType)type)
			{
				case PacketType::TRYCONNECTION:
				{
					std::cout << "TRYCONNECTION 33" << std::endl;

					sf::Packet connectionPacket;
					connectionPacket << (int)PacketType::CHALLENGE;

					// Create challenge
					int number1 = 5;
					int number2 = 5;

					connectionPacket << number1 << number2;

					int solution = number1 * number2;
	
					// Add newConnection to map
					std::pair<sf::IpAddress, unsigned short> _ipAndPort = std::make_pair(remoteIp, remotePort);
					NewConnection _newConnection(remoteIp, remotePort, "", number1, number2, solution);

					_newConnections[_ipAndPort] = _newConnection;

					Send(connectionPacket, remoteIp, remotePort, rcvMessage);
					break;
				}
				case PacketType::RETRYCHALLENGE:
				{
					std::cout << "TRYCONNECTION 34" << std::endl;

					sf::Packet connectionPacket;
					connectionPacket << (int)PacketType::CHALLENGE;

					// Create challenge
					int number1 = 5;
					int number2 = 5;

					connectionPacket << number1 << number2;

					int solution = number1 * number2;

					Send(connectionPacket, remoteIp, remotePort, rcvMessage);
					break;
				}

				case PacketType::CHALLENGE:
				{
					sf::Packet connectionPacket;
					int playerSolution;
					packet >> playerSolution;

					std::pair<sf::IpAddress, unsigned short> _remotePair = std::make_pair(remoteIp, remotePort);

					if (_newConnections[_remotePair].solution == playerSolution)
					{
						_newConnections.erase(_remotePair);
						
						// CUIDAO! Ara mateix tots els clients es diuen igual.
						_clients[_remotePair] = Client("Paco", remoteIp, remotePort, _nextClientId++);

						connectionPacket << (int)PacketType::CANCONNECT;
						Send(connectionPacket, remoteIp, remotePort, rcvMessage);
					}
					else
					{
						connectionPacket << (int)PacketType::CHALLENGEFAILED;
						Send(connectionPacket, remoteIp, remotePort, rcvMessage);
					}
					// Check if the client's solution to challenge is correct:

					break;
				}
				

				case PacketType::MESSAGE:
				{
					break;
				}

				case PacketType::DISCONNECT:
				{
					std::pair<sf::IpAddress, unsigned short> keyToErase;
					keyToErase.first = remoteIp;
					keyToErase.second = remotePort;
					_clients.erase(keyToErase);
					break;
				}

				default:
					break;
			}
		}
		else if (status == sf::Socket::Disconnected)

		{
			std::cout << "Disconnected: " << _port << std::endl;
		}
		else
		{
			// std::cout << "Error al escuchar el puerto: " << _port << std::endl;
		}

		packet >> mssg_temp;
		
		packet.clear();
		rcvMessage->assign(mssg_temp);
	}
}

UDPServerManager::Status UDPServerManager::Connect()
{
	sf::Packet packet;
	packet << (int)PacketType::TRYCONNECTION;

	sf::Socket::Status status = _socket.send(packet, _ip, _port);

	if (status != sf::Socket::Done)
	{
		std::cout << "\t> Status!=Done attempting to connect to:\n\t\t> Server";
		return Status::Error;
	}

	return Status::Done;
}

UDPServerManager::Status UDPServerManager::Listen()
{
	// Poner socket en escucha - Servidor

	// 5000 es el puerto por el que escucha el servidor.
	// El cliente debe conocer la ip del servidor y el puerto por el que escucha.
	Status tempStatus;

	sf::Socket::Status status = _socket.bind(_port);

	if (status == sf::Socket::Done)
	{
		tempStatus = Status::Done;
	}
	else if (status == sf::Socket::Disconnected)
	{
		std::cout << "\t> Status::Disconnected attempting to listen to:\n\t\t> IP: " << _ip << "\n\t\t> Port: " << _port << std::endl;
		tempStatus = Status::Disconnected;
	}
	else
	{
		std::cout << "\t> Error attempting to listen to:\n\t\t> IP: " << _ip << "\n\t\t> Port: " << _port << std::endl;
		tempStatus = Status::Error;
	}

	// Al conectarse un cliente, el socket incoming pasa a ser el que utilizan
	// este cliente y el servidor para comunicarse en exclusive
	
	return tempStatus;
}

void UDPServerManager::Disconnect()
{
	// Desconectar TCP Listener & Socket
	_socket.unbind();
}

unsigned short UDPServerManager::GetPort()
{
	return _port;
}

sf::IpAddress UDPServerManager::GetIp()
{
	return _ip;
}

sf::UdpSocket* UDPServerManager::GetSocket()
{
	return &_socket;
}
