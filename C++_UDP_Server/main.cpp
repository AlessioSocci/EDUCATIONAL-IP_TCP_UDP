#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")

using namespace std;


void main()
{
	WSADATA wsData;

	WORD version = MAKEWORD(2, 2);

	int wsCheck = WSAStartup(version, &wsData); // Initialie winsock and check eventual errors

	if (wsCheck != 0)
	{
		cerr << endl << "WSA Not Initialized" << endl;

		return;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0); // Initialie socket and check eventual error

	sockaddr_in server;

	server.sin_addr.S_un.S_addr = ADDR_ANY; // Use any ip address available

	server.sin_family = AF_INET; // IPv4 address

	server.sin_port = htons(54000); // Convert from little to big endian (little is a standard for pc, big is a standard of the IP)

	if (bind(serverSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		cout << "BIND SOCKET ERROR " << WSAGetLastError() << endl; 

		return;
	}

	sockaddr_in client;

	int size = sizeof(client);

	char buffer[1024];

	while (1)
	{
		ZeroMemory(&client, size); // Clear the client structure
		ZeroMemory(buffer, 1024); // Clear the receive buffer

		int receiveCheck = recv(serverSocket, buffer, 1024, 0);

		if (receiveCheck == SOCKET_ERROR) // SOCKET_ERROR is -1
		{
			cerr << endl << "RX ERROR" << endl;

			return;
		}

		char clientIp[256]; 

		ZeroMemory(clientIp, 256); 

		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		cout << "CLIENT REQUEST-> FROM " << clientIp << " : " << buffer << endl;

		send(serverSocket, buffer, receiveCheck + 1, 0); // server response to the client
	}

	closesocket(serverSocket);

	WSACleanup();
}