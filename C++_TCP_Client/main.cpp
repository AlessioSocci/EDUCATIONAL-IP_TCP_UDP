#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main()
{
	string IPAddress = "127.0.0.1";

	WSADATA wsData;

	WORD version = MAKEWORD(2, 2);

	int wsCheck = WSAStartup(version, &wsData);

	if (wsCheck != 0)
	{
		cerr << endl << "WSA Not Initialized" << wsCheck << endl;

		return;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Initialie socket and check eventual error

	if (clientSocket == INVALID_SOCKET)
	{
		cerr << endl << "Client Socket Not Initialized" << endl;

		WSACleanup();

		return;
	}

	sockaddr_in client;

	client.sin_family = AF_INET;

	client.sin_port = htons(54000); // --> Host to network short. Network work in Big Endian bit string format 

	client.sin_addr.S_un.S_addr = INADDR_ANY;

	inet_pton(AF_INET, IPAddress.c_str(), &client.sin_addr);

	int connectionCheck = connect(clientSocket, (sockaddr*)&client, sizeof(client));

	if (connectionCheck == SOCKET_ERROR) // SOCKET_ERROR is -1
	{
		cerr << endl << "CONNECTION TO SERVER FAILED, ERROR LOG =" << WSAGetLastError() << endl;

		closesocket(clientSocket);

		WSACleanup();

		return;
	}

	char buffer[4096];

	string TXbytes;

	do
	{
		cout << " > ";
	
		getline(cin, TXbytes);

		if (TXbytes.size() > 0)	
		{
			int sendCheck = send(clientSocket, TXbytes.c_str(), TXbytes.size() + 1, 0);

			if (sendCheck != SOCKET_ERROR)
			{
				ZeroMemory(buffer, 4096);

				int RXbytes = recv(clientSocket, buffer, 4096, 0);

				if (RXbytes > 0)
				{
					cout << "SERVER RESPONSE-> " << string(buffer, 0, RXbytes) << endl;
				}
			}
		}

	} while (TXbytes.size() > 0);

	closesocket(clientSocket);

	WSACleanup();
}