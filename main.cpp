
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")



void ServerCheck()
{
	
    while (true)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed\n";
            Sleep(3000);
        }

        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "socket creation failed: " << WSAGetLastError() << "\n";
            WSACleanup();
            Sleep(3000);
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(YOURSERVERPORT);
        inet_pton(AF_INET, "YOURSERVERIP", &serverAddr.sin_addr);

        if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Failed to Connect to the Server with the Error Code: " << WSAGetLastError() << "\n";
            std::cout << "Automatically Restarting the Server\n";
            closesocket(sock);
            WSACleanup();
            system("start C:/Server.exe\n");
            std::cout << "Restarted the Server\n";

          
            Sleep(5000);

            
            if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
            {
                std::cerr << "Server is still marked as offline\n";
            }
            else {
                std::cout << "Server is running\n";
                closesocket(sock);
                WSACleanup();
                Sleep(300000);
                continue;
            }
        }
        else 
        {
            std::cout << "Server is running\n";
        }

        closesocket(sock);
        WSACleanup();
        Sleep(300000);
    }
	
}

int main()
{
	
	ServerCheck();
	return 0;
}


