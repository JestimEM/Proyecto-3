#include "Cliente.h"
#include "Servidor.h"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // Crear una instancia del servidor
    Servidor servidor(5000);

    // Crear una instancia del cliente
    Cliente cliente("localhost", 5000);

    // Crear la ventana SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "Cliente");

    // Crear un string para almacenar los mensajes
    std::string mensajes = "";

    // Crear un texto para mostrar los mensajes
    sf::Text textoMensajes;
    sf::Font fuente;
    if (!fuente.loadFromFile("fonts/arial.ttf")) { // Reemplaza con la ruta a tu fuente
        std::cerr << "No se pudo cargar la fuente." << std::endl;
        return 1;
    }
    textoMensajes.setFont(fuente);
    textoMensajes.setCharacterSize(20);
    textoMensajes.setFillColor(sf::Color::Black);
    textoMensajes.setPosition(10, 10);

    // Bucle principal de la GUI
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    std::string mensaje;
                    std::cout << "Ingrese un mensaje: ";
                    std::getline(std::cin, mensaje); // Obtener mensaje de la consola

                    cliente.enviarMensaje(mensaje);

                    mensajes += "Cliente: " + mensaje + "\n"; // Agregar mensaje al string
                    textoMensajes.setString(mensajes); // Actualizar el texto
                }
            }
        }

        cliente.recibirMensaje(); 

        // Verificar si hay un nuevo mensaje del servidor
        sf::Packet packet;
        if (cliente.socket.receive(packet) == sf::Socket::Done) {
            std::string mensajeRecibido;
            packet >> mensajeRecibido;
            mensajes += "Servidor: " + mensajeRecibido + "\n";
            textoMensajes.setString(mensajes);
        }

        window.clear();
        window.draw(textoMensajes); // Mostrar los mensajes en la ventana
        window.display();
    }

    return 0;
}