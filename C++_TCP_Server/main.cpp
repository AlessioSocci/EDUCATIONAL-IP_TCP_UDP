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

	if(wsCheck != 0)
	{
		cerr << endl << "WSA Not Initialized" << wsCheck << endl;

		return;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Initialie socket and check eventual error
	
	if (serverSocket == INVALID_SOCKET)
	{
		cerr << endl << "Server Socket Not Initialized" << endl;

		return;
	}

	sockaddr_in server; 

	server.sin_family = AF_INET;

	server.sin_port = htons(54000); // --> Host to network short. Network work in Big Endian bit string format 

	server.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(serverSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) // Bind the ip address and port to socket
	{
		cout << "BIND SOCKET ERROR " << WSAGetLastError() << endl;

		return;
	}

	listen(serverSocket, SOMAXCONN); // set listening of the max number of connection 

	sockaddr_in client;

	int size = sizeof(client);

	SOCKET clientSocket = accept(serverSocket, (sockaddr*)&client, &size);

	if (clientSocket == INVALID_SOCKET)
	{
		cerr << endl << "Client Socket Not Initialized" << endl;

		return;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST); // or memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*) &client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << endl << "Host Connected: Port" << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);

		cout << endl << "Host Connected: Port " << ntohs(client.sin_port) << endl;
	}

	closesocket(serverSocket);

	char buffer[4096];

	while (1)
	{	
		ZeroMemory(buffer, 4096);

		int receiveCheck = recv(clientSocket, buffer, 4096, 0);

		if (receiveCheck == SOCKET_ERROR) // SOCKET_ERROR is -1
		{
			cerr << endl << "RX ERROR" << endl;

			return;
		}

		if (receiveCheck == 0)
		{
			cout << endl << "CLIENT DISCONNECTED" << endl;

			break;
		}
				
		char clientIp[256];

		ZeroMemory(clientIp, 256);

		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

		cout << "CLIENT REQUEST-> FROM" << clientIp << " : " << string(buffer, 0, receiveCheck) << endl;

		send(clientSocket, buffer, receiveCheck + 1, 0); // server response to the client
	}

	closesocket(clientSocket);

	WSACleanup();
}