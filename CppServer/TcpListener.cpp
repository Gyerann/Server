#include "TcpListener.h"

TcpListener::TcpListener(std::string ipAddr, int port, msgReceivedHandler handler) 
	:m_ipAddr(ipAddr), m_port(port), msgReceived(handler)
{

}

TcpListener::~TcpListener() {
	CleanUp();
}

//Send message to specified client
void TcpListener::Send(int clientSocket, std::string msg) {
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

//Initialize winsock
bool TcpListener::Init() {
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);

	int wSock = WSAStartup(version, &wsData);

	if (wSock != 0) {
		std::cerr << "[Failed to initialize socket]" << std::endl;
		return 1;
	}
	else {
		std::cout << "[Socket initialized]" << std::endl;
	}

	return wSock == 0;
}

//Main loop
void TcpListener::Run() {

	char msgBuffer[MAX_BUFFER_SIZE];

	while (true) {
		//Crete listener
		SOCKET listener = TcpListener::CreateSocket();
		if (listener == INVALID_SOCKET) {
			break;
		}

		std::cout << "[Listener created]" << std::endl;

		SOCKET client = TcpListener::WaitForConnection(listener);

		if (client != INVALID_SOCKET) {
			std::cout << "[Connection established, closing listener]" << std::endl;
			closesocket(listener);		

			int bytesReceived = 0;

			do {
				memset(msgBuffer, 0, MAX_BUFFER_SIZE);

				bytesReceived = recv(client, msgBuffer, MAX_BUFFER_SIZE, 0);

				if (bytesReceived > 0) {
					if (msgReceived != NULL) {
						msgReceived(this, client, std::string(msgBuffer, 0, bytesReceived));
					}
				}

			} while (bytesReceived > 0);
			
			closesocket(client);

		}

		std::cout << "[Connection closed, creating listener]" << std::endl;
	}
}

//Cleanup
void TcpListener::CleanUp() {
	std::cout << "[Cleanup]" << std::endl;
	WSACleanup();
}

//Create socket
SOCKET TcpListener::CreateSocket() {
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);

	if (listener == INVALID_SOCKET) {
		std::cerr << "[Failed to create socket (invalid socket)]" << std::endl;
		return 1;
	}
	else {
		std::cout << "[Socket created]" << std::endl;

		//Bind ip and port to socket
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);				//Host to network short, networking is big endian so it has to convert
		inet_pton(AF_INET, m_ipAddr.c_str(), &hint.sin_addr);

		int bindReturn = bind(listener, (sockaddr*)&hint, sizeof(hint));

		if (bindReturn != SOCKET_ERROR) {
			//Set sock up for listening
			int listenReturn = listen(listener, SOMAXCONN);
			if (listenReturn == SOCKET_ERROR) {
				std::cout << "[Error while binding listener]" << std::endl;
				return -1;
			}
		}
		else {
			std::cout << "[Error while binding listener]" << std::endl;
			return -1;
		}
	}

	std::cout << "[Listener bound]" << std::endl;
	return listener;
}

//Wait for connection
SOCKET TcpListener::WaitForConnection(SOCKET listener) {
	std::cout << "Waiting for connection..." << std::endl;
	SOCKET client = accept(listener, NULL, NULL);
	return client;
}
