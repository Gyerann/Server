#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

//Incomplete error checking!!!
int main() {

	//Initializing winsock
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);

	int wSock = WSAStartup(version, &wsData);

	if (wSock != 0) {
		cerr << "Failed to initialize socket" << endl;
			return 1;
	}
	else {
		cout << "Socket initialized" << endl;
	}

	//Create socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);

	if (listener == INVALID_SOCKET) {
		cerr << "Failed to create socket (invalid socket)" << endl;
		return 1;
	}
	else {
		cout << "Socket created" << endl;
	}

	//Bind ip and port to socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);				//Host to network short, networking is big endian so it has to convert
	hint.sin_addr.S_un.S_addr = INADDR_ANY;		//Could also use inet_pton

	bind(listener, (sockaddr*)&hint, sizeof(hint));

	//Set sock up for listening
	listen(listener, SOMAXCONN);

	//Wait for connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listener, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];			//Remote name of client
	char service[NI_MAXHOST];		//Service (port) client is connected to

	memset(host, 0, NI_MAXHOST);		//ZeroMemory is defined in winbase but memset is portable
	memset(service, 0, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " connected to port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected to port " << ntohs(client.sin_port) << endl;
	}

	if (clientSocket == INVALID_SOCKET) {
		cerr << "Invalid client socket" << endl;
	}
	else {
		cout << "Client socket connected" << endl;
	}

	//Close listening socket
	closesocket(listener);

	//While: accept and echo
	char buffer[4096];
	int bufferSize = 4096;

	while (true) {
		memset(buffer, 0, bufferSize);

		//Wait for data from client
		int bytesReceived = recv(clientSocket, buffer, bufferSize, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error during recv()" << endl;
			break;
		}
		
		if (bytesReceived == 0) {
			cout << "Client disconnected" << endl;
			break;
		}

		//Echo msg back
		send(clientSocket, buffer, bytesReceived + 1, 0);	//Received message doesnt include a terminator -> + 1

	}

	//Close socket
	closesocket(clientSocket);

	//Cleaning up winsock
	WSACleanup();

	return 0;
}