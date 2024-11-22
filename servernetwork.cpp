#include "servernetwork.hpp"
#include <fstream> 

ServerNetwork::ServerNetwork(unsigned short port) : listen_port(port)
{
    logl("Chat Server Started");

    if (listener.listen(listen_port) != sf::Socket::Done)
    {
        logl("Could not listen");
    }
}

void ServerNetwork::ConnectClients(std::vector<sf::TcpSocket *> *client_array)
{
    while (true)
    {
        sf::TcpSocket *new_client = new sf::TcpSocket();
        if (listener.accept(*new_client) == sf::Socket::Done)
        {
            new_client->setBlocking(false);
            client_array->push_back(new_client);
            logl("Added client " << new_client->getRemoteAddress() << ":" << new_client->getRemotePort() << " on slot " << client_array->size());
        }
        else
        {
            logl("Server listener error, restart the server");
            delete (new_client);
            break;
        }
    }
}

void ServerNetwork::DisconnectClient(sf::TcpSocket *socket_pointer, size_t position)
{
    logl("Client " << socket_pointer->getRemoteAddress() << ":" << socket_pointer->getRemotePort() << " disconnected, removing");
    socket_pointer->disconnect();
    delete (socket_pointer);
    client_array.erase(client_array.begin() + position);
}

void ServerNetwork::BroadcastPacket(sf::Packet &packet, sf::IpAddress exclude_address, unsigned short port)
{
    for (size_t iterator = 0; iterator < client_array.size(); iterator++)
    {
        sf::TcpSocket *client = client_array[iterator];
        if (client->getRemoteAddress() != exclude_address || client->getRemotePort() != port)
        {
            if (client->send(packet) != sf::Socket::Done)
            {
                logl("Could not send packet on broadcast");
            }
        }
    }
}

void ServerNetwork::ReceivePacket(sf::TcpSocket *client, size_t iterator)
{
    sf::Packet packet;
    if (client->receive(packet) == sf::Socket::Disconnected)
    {
        DisconnectClient(client, iterator);  
        return;
    }

    if (packet.getDataSize() > 0)
    {
        sf::Uint8 packetType;
        packet >> packetType;

        if (packetType == 0) { // Mensaje de texto
            std::string received_message;
            packet >> received_message;
            packet.clear();

            packet << static_cast<sf::Uint8>(0) << received_message << client->getRemoteAddress().toString() << client->getRemotePort();

            BroadcastPacket(packet, client->getRemoteAddress(), client->getRemotePort());
            logl(client->getRemoteAddress().toString() << ":" << client->getRemotePort() << " '" << received_message << "'");

        } else if (packetType == 1) { // Archivo
        std::string filename;
        sf::Uint64 fileSize; 
        packet >> filename >> fileSize; 

        // Reenviar el encabezado del archivo (nombre y tamaño)
        BroadcastPacket(packet, client->getRemoteAddress(), client->getRemotePort()); 

        // Recibir los datos del archivo del cliente emisor
        std::streampos receivedBytes = 0;
        while (receivedBytes < fileSize) {
            sf::Packet dataPacket;
            client->receive(dataPacket);
            if (dataPacket.getDataSize() > 0) {
                // Reenviar el paquete de datos a los demás clientes
                BroadcastPacket(dataPacket, client->getRemoteAddress(), client->getRemotePort()); 
                receivedBytes += dataPacket.getDataSize();
            }
        }

        logl("Archivo recibido de " << client->getRemoteAddress().toString() << ":" << client->getRemotePort() << ": " << filename);
        }
    }
}

void ServerNetwork::ManagePackets() {
    while (true)
    {
        for (size_t iterator = 0; iterator < client_array.size(); iterator++)
        {
            ReceivePacket(client_array[iterator], iterator);
        }

        std::this_thread::sleep_for((std::chrono::milliseconds)250);
    }
}

void ServerNetwork::Run()
{
    std::thread connetion_thread(&ServerNetwork::ConnectClients, this, &client_array);

    ManagePackets();
}