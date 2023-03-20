// flatbuffer-tcp.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "basic_tcp_server.h"
#include "basic_tcp_client.h"
#include "property.h"

#include <thread>

constexpr unsigned int buflen = 1024;

int main(int argc, char* argv[])
{
    char buffer[buflen];
    BasicTcpServer server;
    std::thread server_thread([&]() { server.Listen<Property>(buffer, buflen); });
    
    BasicTcpClient client;
    // create some dummy tree that increases in size to send over
    auto root = std::make_shared<Property>("root", 0, 0);
    auto property = root;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            property->SetSubproperty(std::to_string(i * 5 + j), i, j);
        }
        property = property->GetSubproperties().begin()->second;

        client.Send(root, buflen);
        Sleep(1000);
    }
    client.Shutdown();

    server_thread.join();

    return 0;
}
