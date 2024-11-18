#ifndef CLIENTE_H
#define CLIENTE_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

class Cliente {
public:
    Cliente(const std::string& ip, unsigned short puerto);
    void ejecutar();
    void conectar();
    void enviarMensaje(const std::string& mensaje);
    void recibirMensaje();

    sf::TcpSocket socket;
    std::string ipServidor;
    unsigned short puertoServidor;
};

#endif