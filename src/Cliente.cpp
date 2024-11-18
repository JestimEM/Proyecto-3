#include "Cliente.h"

Cliente::Cliente(const std::string& ip, unsigned short puerto) :
    ipServidor(ip), puertoServidor(puerto) 
{
    conectar();
}

void Cliente::conectar() {
    sf::Socket::Status status = socket.connect(ipServidor, puertoServidor);
    if (status != sf::Socket::Done) {
        std::cerr << "Error al conectar al servidor." << std::endl;
        // Manejar el error, por ejemplo, mostrando un mensaje en la GUI
    } else {
        std::cout << "Conectado al servidor." << std::endl;
    }
}

void Cliente::enviarMensaje(const std::string& mensaje) {
    sf::Packet packet;
    packet << mensaje;
    if (socket.send(packet) != sf::Socket::Done) {
        std::cerr << "Error al enviar mensaje al servidor." << std::endl;
    }
}

void Cliente::recibirMensaje() {
    sf::Packet packet;
    if (socket.receive(packet) == sf::Socket::Done) {
        std::string mensajeRecibido;
        packet >> mensajeRecibido;
        std::cout << "Mensaje del servidor: " << mensajeRecibido << std::endl;
        // El mensaje se mostrará en la GUI en el main.cpp
    }
}