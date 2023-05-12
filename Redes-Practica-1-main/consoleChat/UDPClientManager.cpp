#include "UDPClientManager.hpp"

#define PKT_LOSS_PROB 5 // 5 sobre 1000

UDPClientManager::Status UDPClientManager::Send(sf::Packet& packet, sf::IpAddress ip, unsigned short port, std::string* sendMessage)
{
	// Fill packet with data:
	packet << *sendMessage;
	sf::Socket::Status status;
	// Send the data to the socket:
	if (probLossManager.generate_prob() > PKT_LOSS_PROB)
		status = _socket.send(packet, ip, port);
	else
		status = sf::Socket::Status::Disconnected;

	if (status != sf::Socket::Done)
	{
		std::cout << "\t> A packet has been lost.";
		return Status::Error;
	}


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
								   // per aix� ho canviem per Status::Error.
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

void UDPClientManager::Receive(sf::Packet& packet, std::string* rcvMessage)  // No ho podem passar per refer�ncia,
																																// el valor de rcvMessage no s'aplica fora de la funci�.
{
	sf::IpAddress remoteIp;
	unsigned short remotePort;

	std::string mssg_temp;

	while (true)
	{
		sf::Socket::Status status = _socket.receive(packet, remoteIp, remotePort);

		if (status == sf::Socket::Done)
		{
			std::cout << "HA ARRIBAT UN PAQUET AL CLIENT" << std::endl;

			int type;
			packet >> type;

			// Aqu� gestionarem tots els diferents casos possibles de missatge (PacketType):
			switch ((PacketType)type)
			{

				case PacketType::CANCONNECT: // Potser aix� s'ha de tocar
				{
					std::cout << "\t> Connection established." << std::endl;
					break;
				}

				case PacketType::CANNOTCONNECT:
				{
					std::cout << "\t> Could not connect due to the port being full." << std::endl;
					std::cout << "\t> Increasing port and trying again." << std::endl;

					_port++;
					Connect();
					break;
				}

				case PacketType::CHALLENGE:
				{
					std::cout << "\t> Recieved challenge." << std::endl;
					std::cout << "\t> Attempting to solve." << std::endl;

					int number1;
					int number2;

					packet >> number1 >> number2;
					std::cout << "Solve the following math operation: " << number1 << "*" << number2 << std::endl;
					int clientSolution;
					solvingChallenge = true;
					std::cin >> clientSolution;
					
					sf::Packet challengeSolutionPacket;
					challengeSolutionPacket << (int)PacketType::CHALLENGE << clientSolution;
					solvingChallenge = false;
					Send(challengeSolutionPacket, remoteIp, remotePort, rcvMessage);
					//_socket.send(challengeSolutionPacket, remoteIp, remotePort);

					break;
				}

				case PacketType::CHALLENGEFAILED:
				{
					std::cout << "ChallengeFailed" << std::endl;
					sf::Packet retryChallengePacket;
					retryChallengePacket << (int)PacketType::RETRYCHALLENGE;

					Send(retryChallengePacket, remoteIp, remotePort, rcvMessage);
					//_socket.send(retryChallengePacket, remoteIp, remotePort);

					break;
				}

				case PacketType::MESSAGE:
				{
					packet >> mssg;
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
			std::cout << "TEST" << std::endl;

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

UDPClientManager::Status UDPClientManager::Connect()
{
	sf::Socket::Status status = _socket.bind(_port);

	while (status != sf::Socket::Status::Done)
	{
		_port++;

		status = _socket.bind(_port);
	}

	sf::Packet packet;
	packet << (int)PacketType::TRYCONNECTION;

	if(probLossManager.generate_prob() > PKT_LOSS_PROB)
		_socket.send(packet, sf::IpAddress("127.0.0.1"), 5000);
	else
		status = sf::Socket::Status::Disconnected;

	if (status != sf::Socket::Done)
	{
		std::cout << "\t> A packet has been lost.";
		return Status::Error;
	}


	return Status::Done;
}

void UDPClientManager::Disconnect()
{
	// Desconectar TCP Listener & Socket
	_socket.unbind();
}

unsigned short UDPClientManager::GetLocalPort()
{
	return _port;
}

sf::IpAddress UDPClientManager::GetIp()
{
	return _ip;
}

sf::UdpSocket* UDPClientManager::GetSocket()
{
	return &_socket;
}
