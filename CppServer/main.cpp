#include "TcpListener.h"

void Listener_MessageReceived(TcpListener* listener, int client, std::string msg);

int main() {

	TcpListener server("127.0.0.1", 54000, Listener_MessageReceived);

	if (server.Init()) {
		server.Run();
	}

	return 0;
}

void Listener_MessageReceived(TcpListener* listener, int client, std::string msg) {

}