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

void UDPServerManager::Receive(sf::Packet& packet, sf::IpAddress remoteIp, unsigned short remotePort, std::string* rcvMessage)  // No ho podem passar per referència,
																																// el valor de rcvMessage no s'aplica fora de la funció.
{
	std::string mssg_temp;

	while (true)
	{
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
					break;
				}

				case PacketType::CANCONNECT:
				{
					break;
				}

				case PacketType::CANNOTCONNECT:
				{
					break;
				}

				case PacketType::MESSAGE:
				{
					break;
				}

				case PacketType::DISCONNECT:
				{
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

unsigned short UDPServerManager::GetLocalPort()
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
