#pragma once
#include <iostream>
#include <WS2tcpip.h>
//Callback to data received
#pragma comment (lib, "ws2_32.lib")

#define MAX_BUFFER_SIZE (4096)

//Forward declaration
class TcpListener;

//Callback to data received
typedef void (*msgReceivedHandler)(TcpListener* listener, int socketId, std::string msg);

class TcpListener {
public:
	TcpListener(std::string ipAddr, int port, msgReceivedHandler handler);

	~TcpListener();

	//Send message to specified client
	void Send(int clientSocket, std::string msg);

	//Initialize winsock
	bool Init();

	//Main loop
	void Run();

	//Cleanup
	void CleanUp();

private:
	std::string			m_ipAddr;
	int					m_port;
	msgReceivedHandler	msgReceived;

	SOCKET CreateSocket();

	SOCKET WaitForConnection(SOCKET listener);
};