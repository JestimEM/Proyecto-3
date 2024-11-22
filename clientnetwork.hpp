#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream> 
#include <SFML/Network.hpp>

#define logl(x, fullpath) std::cout << x << fullpath << std::endl
#define log(x) std::cout << x


class ClientNetwork{
    sf::TcpSocket socket;
    sf::Packet last_packet;

    bool isConnected = false;
public:
    ClientNetwork();
    void Connect(const char *, unsigned short);
    void ReceivePackets(sf::TcpSocket *);
    void SendPacket(sf::Packet &);
    void SendFile(const std::string&); // Corrección: solo un parámetro
    void Run();
};