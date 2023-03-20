#pragma once

#include "serializable.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

constexpr const char* DEFAULT_CLIENT_ADDRESS = "localhost";
constexpr const char* DEFAULT_CLIENT_PORT = "27015";

class BasicTcpClient
{
public:
    BasicTcpClient()
    {
        std::cout << "[CLIENT] Initialising client \n";

        WSADATA wsa_data;
        connect_socket = INVALID_SOCKET;
        struct addrinfo* result = NULL,
            * ptr = NULL,
            hints;

        // Initialise Winsock
        i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (i_result != 0)
        {
            std::cerr << "[CLIENT] WSAStartup failed with error: " << i_result << '\n';
            exit(1);
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve server address and port
        i_result = getaddrinfo(DEFAULT_CLIENT_ADDRESS, DEFAULT_CLIENT_PORT, &hints, &result);
        if (i_result != 0)
        {
            std::cerr << "[CLIENT] getaddrinfo failed with error: " << i_result << '\n';
            WSACleanup();
            exit(1);
        }

        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
        {
            // Create a socket for connecting to server
            connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (connect_socket == INVALID_SOCKET)
            {
                std::cerr << "[CLIENT] socket failed with error: " << WSAGetLastError() << '\n';
                WSACleanup();
                exit(1);
            }

            // Connect to server
            i_result = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (i_result == SOCKET_ERROR)
            {
                closesocket(connect_socket);
                connect_socket = INVALID_SOCKET;
                continue;
            }
            break;
        }

        freeaddrinfo(result);

        if (connect_socket == INVALID_SOCKET)
        {
            std::cerr << "[CLIENT] Unable to connect to server! \n";
            WSACleanup();
            exit(1);
        }

        std::cout << "[CLIENT] Initialised client \n";
    }

    ~BasicTcpClient()
    {
        std::cout << "[CLIENT] Closing client \n";
        closesocket(connect_socket);
        WSACleanup();
    }

    template<typename T>
    void Send(std::shared_ptr<T> object, size_t buflen)
    {
        flatbuffers::FlatBufferBuilder builder(buflen);
        auto* serialized = Serializable::Serialize<T>(builder, object);

        int i_result = send(connect_socket, serialized, builder.GetSize(), 0);
        if (i_result == SOCKET_ERROR)
        {
            std::cerr << "[CLIENT] send failed with error: " << WSAGetLastError() << '\n';
            closesocket(connect_socket);
            WSACleanup();
            exit(1);
        }
        std::cout << "[CLIENT] Bytes Sent: " << i_result << '\n';
    }

    void Shutdown()
    {
        std::cout << "[CLIENT] Shutting down client\n";
        // shutdown the connection since no more data will be sent
        i_result = shutdown(connect_socket, SD_SEND);
        if (i_result == SOCKET_ERROR)
        {
            std::cerr << "[CLIENT] shutdown failed with error: " << WSAGetLastError() << '\n';
            closesocket(connect_socket);
            WSACleanup();
            exit(1);
        }
    }

private:
    SOCKET connect_socket;
    int i_result;
};
