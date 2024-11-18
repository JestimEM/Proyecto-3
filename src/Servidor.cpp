#include "Servidor.h"

Servidor::Servidor(unsigned short puerto) : puertoServidor(puerto) {
    if (listener.listen(puertoServidor) != sf::Socket::Done) {
        std::cerr << "Error al iniciar el servidor." << std::endl;
        // Manejar el error
    } else {
        std::cout << "Servidor iniciado en el puerto " << puertoServidor << std::endl;
    }
}

void Servidor::ejecutar() {
    while (true) {
        aceptarConexiones();
    }
}

void Servidor::aceptarConexiones() {
    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Done) {
        std::cerr << "Error al aceptar la conexión." << std::endl;
    } else {
        std::cout << "Cliente conectado." << std::endl;
        manejarCliente(client);
    }
}

void Servidor::manejarCliente(sf::TcpSocket& cliente) {
    sf::Packet packet;
    if (cliente.receive(packet) == sf::Socket::Done) {
        std::string mensajeRecibido;
        packet >> mensajeRecibido;
        std::cout << "Mensaje del cliente: " << mensajeRecibido << std::endl;

        // Procesar el mensaje (descifrar, verificar errores, etc.)
        // ...

        // Enviar una respuesta al cliente
        std::string respuesta = "Mensaje del servidor";
        packet.clear();
        packet << respuesta;
        if (cliente.send(packet) != sf::Socket::Done) {
            std::cerr << "Error al enviar respuesta al cliente." << std::endl;
        }
    }
    cliente.disconnect();
}