#include "UdpNetworkManager.hpp"

UDPSocketManager::Status UDPSocketManager::Send(sf::Packet& packet, std::string* sendMessage)
{
	// Fill packet with data:
	packet << *sendMessage;

	// Send the data to the socket:
	sf::Socket::Status status = _socket.send(packet);

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

void UDPSocketManager::Receive(sf::Packet& packet, std::string* rcvMessage) // No ho podem passar per referència,
																			// el valor de rcvMessage no s'aplica fora de la funció.
{
	std::string mssg_temp;

	while (true)
	{
		sf::Socket::Status status = _socket.receive(packet);

		if (status == sf::Socket::Done)
		{

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

		// Check if the message is "exit" or "Exit":
		//if (*rcvMessage == "exit" || *rcvMessage == "Exit")
			//return;
	}
}

UDPSocketManager::Status UDPSocketManager::Connect()
{
	sf::Socket::Status status = _socket.connect(_ip, _port, sf::seconds(5.f));

	if (status != sf::Socket::Done)
	{
		std::cout << "\t> Status!=Done attempting to connect to:\n\t\t> IP: " << _ip << "\n\t\t> Port: " << _port;
		return Status::Error;
	}

	return Status::Done;
}

UDPSocketManager::Status UDPSocketManager::Listen()
{
	// Poner socket en escucha - Servidor

	// 5000 es el puerto por el que escucha el servidor.
	// El cliente debe conocer la ip del servidor y el puerto por el que escucha.
	Status tempStatus;

		sf::Socket::Status status = _dispatcher.listen(_port);

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

void UDPSocketManager::Disconnect()
{
	// Desconectar TCP Listener & Socket
	_dispatcher.close();
	_socket.disconnect();
}

void UDPSocketManager::SocketSelectorFunctionality()
{
	selector.add(_dispatcher); // Nuestro dispatcher creemos que es el listener de la documentacion

	while (true)
	{
		if (selector.wait())
		{
			if (selector.isReady(_dispatcher))
			{
				sf::TcpSocket* client = new sf::TcpSocket;
				if (_dispatcher.accept(*client) == sf::Socket::Done)
				{
					//cin of the name and store it 
					clients.push_back(client);
					selector.add(*client);
				}
				else
				{
					delete client;
				}
			}
			else
			{
				int index = 0;
				int index1 = 0;
				for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
				{
					index++;
					sf::TcpSocket& client = **it;
					if (selector.isReady(client))
					{
						sf::Packet packet;
						if (client.receive(packet) == sf::Socket::Done)
						{
							for (std::list<sf::TcpSocket*>::iterator it2 = clients.begin(); it2 != clients.end(); ++it2)
							{
								index1++;
								if (index1 == index)
								{
									continue;
								}
								else
								{
									sf::Socket::Status status = (*it2)->send(packet);
								}
							}
							index1 = 0;
						}
						else
						{
							auto tempIt = it;

							if (it != clients.begin()) {
								--it;
								clients.erase(tempIt);
							}
							else {
								clients.erase(tempIt);
								break;
							}
							
						}
					}
				}
			}

		}

	}

}

unsigned short UDPSocketManager::GetLocalPort()
{
	return _port;
}

sf::IpAddress UDPSocketManager::GetIp()
{
	return _ip;
}

sf::TcpSocket* UDPSocketManager::GetSocket()
{
	return &_socket;
}
