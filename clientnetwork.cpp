#include "clientnetwork.hpp"
#include <fstream> 

ClientNetwork::ClientNetwork()
{
    logl("Chat Client Started");
}

void ClientNetwork::Connect(const char *address, unsigned short port)
{
    if (socket.connect(address, port) != sf::Socket::Done)
    {
        logl("Could not connect to the server\n");
    }
    else
    {
        isConnected = true;
        logl("Connected to the server\n");
    }
}

void ClientNetwork::ReceivePackets(sf::TcpSocket *socket)
{
    bool receivingFile = false;  // Variable para controlar si se está recibiendo un archivo
    std::string filename;        // Variable para almacenar el nombre del archivo
    sf::Uint64 fileSize = 0;     // Variable para almacenar el tamaño del archivo
    std::ofstream outputFile;    // Declarar outputFile fuera del if

    while (true)
    {
        sf::Packet packet;
        if (socket->receive(packet) == sf::Socket::Done)
        {
            sf::Uint8 packetType;
            packet >> packetType;

            if (packetType == 0) { // Mensaje de texto
                std::string received_string;
                std::string sender_address;
                unsigned short sender_port;
                packet >> received_string >> sender_address >> sender_port;
                logl("From (" << sender_address << ":" << sender_port << "): " << received_string);

            } else if (packetType == 1) { // Archivo - INICIO DE LA TRANSFERENCIA
                packet >> filename >> fileSize;

                // Solicitar al cliente receptor la carpeta de destino
                std::cout << "Ingrese la carpeta donde desea guardar el archivo: ";
                std::string destination;
                std::getline(std::cin, destination);

                // Crear la ruta completa del archivo en el cliente receptor
                std::string fullpath = destination + "/" + filename;

                // Crear el archivo
                outputFile.open(fullpath, std::ios::binary);
                if (!outputFile.is_open()) {
                    logl("Error al crear el archivo: " << fullpath);
                    continue; // Continuar con la siguiente iteración del bucle
                }

                std::cout << "Esperando recibir datos del archivo..." << std::endl;
                receivingFile = true; // Indicar que se está recibiendo un archivo
            } 
        }

        // Recibir y escribir los datos del archivo SI SE ESTÁ RECIBIENDO UN ARCHIVO
        if (receivingFile) { 
            std::streampos receivedBytes = 0;
            char buffer[4096];

            while (receivedBytes < fileSize) {
                sf::Packet dataPacket;
                if (socket->receive(dataPacket) != sf::Socket::Done) { 
                    std::cerr << "Error al recibir el paquete de datos." << std::endl;
                    break; 
                }

                if (dataPacket.getDataSize() > 0) {
                    std::cout << "Recibido bloque de datos con tamaño: " << dataPacket.getDataSize() << std::endl;

                    // Obtener el tamaño de los datos recibidos
                    size_t dataSize = dataPacket.getDataSize();

                    // Escribir los datos en el archivo
                    outputFile.write(reinterpret_cast<const char*>(dataPacket.getData()), dataSize);

                    // Actualizar la cantidad de bytes recibidos
                    receivedBytes += dataSize;
                }
            }

                if (receivedBytes == fileSize) { // Verificar si se recibió todo el archivo
                    std::cout << "Archivo recibido completamente." << std::endl;
                    logl("Archivo recibido: " << fullpath);
                    receivingFile = false; // Restablecer la variable para la próxima transferencia
                    outputFile.close(); // Cerrar el archivo después de recibirlo
                }
            }
        }

        std::this_thread::sleep_for((std::chrono::milliseconds)250);
    }


void ClientNetwork::SendPacket(sf::Packet &packet)
{
    if (packet.getDataSize() > 0 && socket.send(packet) != sf::Socket::Done) {
        logl("Could not send packet");
    } else {
        std::cout << "Paquete enviado con tamaño: " << packet.getDataSize() << std::endl;
    }
}

// Modificación: solo recibe la ruta del archivo
void ClientNetwork::SendFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        logl("Error al abrir el archivo");
        return;
    }

    // Obtener el tamaño del archivo
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Obtener solo el nombre del archivo
    std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);

    // Crear el encabezado (sin la carpeta de destino)
    sf::Packet packet;
    packet << static_cast<sf::Uint8>(1) << filename << static_cast<sf::Uint64>(fileSize); 
    SendPacket(packet);

    // Enviar el archivo en bloques
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        sf::Packet packet;
        packet.append(buffer, file.gcount());
        SendPacket(packet);
    }

    if (file.gcount() > 0) { 
        sf::Packet packet;
        packet.append(buffer, file.gcount());
        SendPacket(packet);
    }

    logl("Archivo enviado: " << filename);
}

void ClientNetwork::Run()
{
    std::thread reception_thred(&ClientNetwork::ReceivePackets, this, &socket);

    while (true)
    {
        if (isConnected)
        {
            std::string user_input;
            std::getline(std::cin, user_input);

            if (user_input.length() < 1)
                continue;

            if (user_input.substr(0, 5) == "/file") { // Comando para enviar archivos
                std::cout << "Ingrese la ruta completa del archivo: ";
                std::string filepath;
                std::getline(std::cin, filepath);

                SendFile(filepath); // Llama a SendFile con solo la ruta del archivo
            } else { // Mensaje de texto
                sf::Packet reply_packet;
                reply_packet << static_cast<sf::Uint8>(0) << user_input;
                SendPacket(reply_packet);
            }
        }
    }
}