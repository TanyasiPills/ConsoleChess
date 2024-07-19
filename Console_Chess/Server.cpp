#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <string>

#include "Server.h"
#include "Console_Chess.h"

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

#define PORT 48240
#define BUFF 512

string hihi;
const char* str;
int strLength;
SOCKET clientSock;
string mode;
string ip;
sockaddr_in client;
SOCKET sock;
char host[NI_MAXHOST];
char service[NI_MAXHOST];
char buffer[4096];


void message();
void getmessage();

int initalize_server() {
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);

	int wsok = WSAStartup(version, &wsData);
	if (wsok == 0) cout << "Started ws..." << endl;
	else cout << "Can't start ws..." << endl;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) cout << "sad sock...";
	cout << "Runtime mode: ";

	cin >> mode;

	if (mode == "host") {
		turn = 0;
		side = 0;

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(PORT);
		hint.sin_addr.S_un.S_addr = INADDR_ANY;

		bind(sock, (sockaddr*)&hint, sizeof(hint));
		listen(sock, SOMAXCONN);

		int client_size = sizeof(client);

		clientSock = accept(sock, (sockaddr*)&client, &client_size);

		cout << "succesfully connected" << endl;

		closesocket(sock);
	}
	else if (mode == "join") {
		turn = 1;
		side = 1;

		cout << "Ip: ";
		cin >> ip;

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(PORT);
		inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);

		clientSock = socket(AF_INET, SOCK_STREAM, 0);

		int connection_result = connect(clientSock, (sockaddr*)&hint, sizeof(hint));
	}
	else if (mode == "skip") {
		return 0;
	}
	else if (mode == "exit") {
		ExitProcess(0);
	}
	
	cout << "chop chop";
}

void message() {
	while (true) {
		getline(cin, hihi);
		str = hihi.c_str();
		strLength = strlen(str) + 1;
		if(Game(hihi, false)) send(clientSock, str, strLength, 0);
	}
}

void getmessage() {
	while (true) {
		try {
			ZeroMemory(buffer, 4096);
			int bytesReceived = recv(clientSock, buffer, 4096, 0);
			string recvMess = string(buffer, bytesReceived);
			if (bytesReceived > 0) {
				Game(recvMess, true);
			}
			else if (bytesReceived == 0) {
				Game("/Game disconnected", true);
			}
			else {
				Game("/Game disconnected", true);
			}
		}
		catch (int e) {
			OutputDebugStringA("server_error\n");
		}
	}
}