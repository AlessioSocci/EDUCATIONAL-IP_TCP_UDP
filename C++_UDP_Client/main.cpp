#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")

using namespace std;


void main(int argc, char* argv[])
{
	string IPAddress = "127.0.0.1";

	WSADATA wsData;

	WORD version = MAKEWORD(2, 2);

	int wsCheck = WSAStartup(version, &wsData); // Initialie winsock and check eventual errors

	if (wsCheck != 0)
	{
		cerr << endl << "WSA Not Initialized" << endl;

		return;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0); // Initialie socket and check eventual error

	sockaddr_in client;

	client.sin_addr.S_un.S_addr = ADDR_ANY; // Use any ip address available

	client.sin_family = AF_INET; // IPv4 address

	client.sin_port = htons(54000); // Convert from little to big endian (little is a standard for pc, big is a standard of the IP)

	inet_pton(AF_INET, IPAddress.c_str(), &client.sin_addr);

	string TXbytes;
	
	do
	{
		cout << " > ";

		getline(cin, TXbytes);

		int sendCheck = sendto(clientSocket, TXbytes.c_str(), TXbytes.size() + 1, 0, (sockaddr*)&client, sizeof(client));

		if (sendCheck == SOCKET_ERROR)
		{
			cout << "ERROR " << WSAGetLastError() << endl;
		}

	} while (TXbytes.size() > 0);

	closesocket(clientSocket);

	WSACleanup();
}