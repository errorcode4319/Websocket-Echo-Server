#include"ws-server.h"

#include"ws-secure-util.h"
#include <iomanip>

constexpr const char*	HOST = "0.0.0.0";
constexpr int			PORT = 7070;

int main() {
	
	ws::WSServer ws_server;
	ws_server.init();
	ws_server.run("0.0.0.0", 7070);
	std::cout << "Websocket Echo Server " << HOST << ':' << PORT << std::endl;
	std::string input;
	std::getline(std::cin, input);

	return 0;
}