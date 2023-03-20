#pragma once

#include "serializable.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

constexpr const char* DEFAULT_SERVER_PORT = "27015";

class BasicTcpServer
{
public:
    BasicTcpServer()
    {
        std::cout << "[SERVER] Initialising server\n";
        WSADATA wsa_data;

        listen_socket = INVALID_SOCKET;
        client_socket = INVALID_SOCKET;

        struct addrinfo* result = NULL;
        struct addrinfo hints;

        // Initialise Winsock
        i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (i_result != 0)
        {
            std::cerr << "[SERVER] WSAStartup failed with error: " << i_result << '\n';
            exit(1);
        }

        // Set address information
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        i_result = getaddrinfo(NULL, DEFAULT_SERVER_PORT, &hints, &result);
        if (i_result != 0)
        {
            std::cerr << "[SERVER] getaddrinfo failed with error: " << i_result << '\n';
            WSACleanup();
            exit(1);
        }

        // Create a SOCKET for connecting to server
        listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (listen_socket == INVALID_SOCKET)
        {
            std::cerr << "[SERVER] socket failed with error: " << WSAGetLastError() << '\n';
            freeaddrinfo(result);
            WSACleanup();
            exit(1);
        }

        // Setup the TCP listening socket
        i_result = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
        if (i_result == SOCKET_ERROR)
        {
            std::cerr << "[SERVER] bind failed with error: " << WSAGetLastError() << '\n';
            freeaddrinfo(result);
            closesocket(listen_socket);
            WSACleanup();
            exit(1);
        }

        // no longer need address information
        freeaddrinfo(result);

        // start listening for new clients attempting to connect
        i_result = listen(listen_socket, SOMAXCONN);
        if (i_result == SOCKET_ERROR)
        {
            std::cerr << "[SERVER] listen failed with error: " << WSAGetLastError() << '\n';
            closesocket(listen_socket);
            WSACleanup();
            exit(1);
        }

        std::cout << "[SERVER] Initialised server\n";
    }

    ~BasicTcpServer()
    {
        std::cout << "[SERVER] Closing server\n";
        closesocket(client_socket);
        WSACleanup();
    }

    template<typename T>
    void Listen(char* buffer, int buflen)
    {
        do
        {
            client_socket = accept(listen_socket, NULL, NULL);
        } while (client_socket == INVALID_SOCKET);

        closesocket(listen_socket);

        std::cout << "[SERVER] Listening to client\n";
        do
        {
            i_result = recv(client_socket, buffer, buflen, 0);
            if (i_result > 0)
            {
                std::cout << "[SERVER] Bytes received: " << i_result << '\n';

                // Echo the buffer back to the sender
                int i_send_result = send(client_socket, buffer, i_result, 0);
                if (i_send_result == SOCKET_ERROR)
                {
                    std::cerr << "[SERVER] send failed with error: " << WSAGetLastError() << '\n';
                    closesocket(client_socket);
                    WSACleanup();
                    exit(1);
                }
                std::cout << "[SERVER] Bytes sent: " << i_send_result << '\n';

                auto deserialized = Serializable::Deserialize<T>(buffer);
                deserialized->Print();
            }
            else if (i_result == 0)
            {
                std::cout << "[SERVER] Connection closing...\n";
            }
        } while (i_result > 0);
    }

private:
    SOCKET listen_socket;
    SOCKET client_socket;
    int i_result;
};

