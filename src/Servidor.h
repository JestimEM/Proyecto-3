#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <SFML/Network.hpp>
#include <iostream>

class Servidor {
public:
    Servidor(unsigned short puerto);
    void ejecutar();

private:
    void aceptarConexiones();
    void manejarCliente(sf::TcpSocket& cliente);

    sf::TcpListener listener;
    unsigned short puertoServidor;
};

#endif