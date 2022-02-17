#include"ws-server.h"

int main() {

	ws::WebSocketServer ws_server;
	ws_server.init();
	ws_server.run();

	std::string input;
	std::getline(std::cin, input);

	return 0;
}