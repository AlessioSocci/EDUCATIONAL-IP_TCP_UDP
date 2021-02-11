#include <iostream>
#include <string>
#include<sstream>
#include<vector>
#include<thread>
#include <WS2tcpip.h>

#pragma comment(lib, "WS2_32.lib")

using namespace std;


void main()
{
	WSADATA wsData;

	WORD version = MAKEWORD(2, 2);

	struct client_struct
	{
		char client_ip[1025];
		SOCKET sock;
	};

	vector<client_struct> client_vector;

	int wsCheck = WSAStartup(version, &wsData); // Initialie winsock and check eventual errors

	if (wsCheck != 0)
	{
		cerr << endl << "WSA Not Initialized" << wsCheck << endl;

		return;
	}

	thread addHost([&client_vector]()
		{
			sockaddr_in server;

			server.sin_family = AF_INET;

			server.sin_port = htons(54000);

			server.sin_addr.S_un.S_addr = INADDR_ANY;

			SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

			if (bind(serverSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) // Bind the ip address and port to socket
			{
				cout << "BIND SOCKET ERROR " << WSAGetLastError() << endl;

				return;
			}

			listen(serverSocket, SOMAXCONN); // set listening of the max number of connection 

			sockaddr_in client;

			int size = sizeof(client);

			SOCKET clientSocket;

			u_long non_blocking = true;

			while (1)
			{
				clientSocket = accept(serverSocket, (sockaddr*)&client, &size);

				if (clientSocket == INVALID_SOCKET)
				{
					cerr << endl << "Client Socket Not Initialized" << endl;

					return;
				}
				else
				{
					ioctlsocket(clientSocket, FIONBIO, &non_blocking);

					client_struct new_client;

					new_client.sock = clientSocket;

					inet_ntop(AF_INET, &client.sin_addr, new_client.client_ip, NI_MAXHOST);

					char service[NI_MAXHOST];

					ZeroMemory(service, NI_MAXHOST);

					if (getnameinfo((sockaddr*)&client, sizeof(client), new_client.client_ip, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
					{
						cout << endl << "Host Connected: Port  " << service << endl;
					}
					else
					{
						inet_ntop(AF_INET, &client.sin_addr, new_client.client_ip, NI_MAXHOST);

						cout << endl << "Host Connected: Port  " << ntohs(client.sin_port) << endl;
					}

					client_vector.push_back(new_client);

					std::cout << "Client " << new_client.client_ip << " - Host number : " << client_vector.size() - 1 << std::endl;

				}

			}

		});


	thread checkHost([&client_vector]()
		{
			char buffer[4096];

			while (1)
			{
				for (int i = 0; i < client_vector.size(); i++)
				{
					memset(buffer, 0, sizeof(buffer));

					int sendCheck = send(client_vector[i].sock, "", 1, 0);

					int receiveCheck = recv(client_vector[i].sock, buffer, sizeof(buffer), 0);

					if (sendCheck != 0)
					{
						cout << endl << "Connection check : " << client_vector[i].client_ip << " - Host number : " << i << " - CONNECTED ";
					}
					if (sendCheck < 0)
					{
						cout << endl << "Connection check : " << client_vector[i].client_ip << " - Host number : " << i << " - !! DISCONNECTED !! " << std::endl;

						client_vector.erase(client_vector.begin() + i);

						cout << "Host number reconfigured";
					}
				}

				this_thread::sleep_for(std::chrono::seconds(10));
			}
		});


	thread receive([&client_vector]()
		{
			char buffer[4096];

			while (1)
			{
				for (int i = 0; i < client_vector.size(); i++)
				{
					memset(buffer, 0, sizeof(buffer));

					int receiveCheck = recv(client_vector[i].sock, buffer, sizeof(buffer), 0);

					if (receiveCheck == 0)
					{
						cout << endl << "CLIENT DISCONNECTED" << endl;
					}
					else if (receiveCheck > 1)
					{
						cout << endl << "Received from : " << client_vector[i].client_ip << " - Host number : " << i << " --> " << string(buffer, 0, receiveCheck);

						send(client_vector[i].sock, buffer, 4096, 0);
					}
				}

			}
		});

	string msg;

	while (1)
	{
		getline(cin, msg);

		if (msg == "stop")
		{
			break;
		}
	}

	WSACleanup();
}