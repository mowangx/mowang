
#include "stdio.h"
#include <iostream>

#include "game_server.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return 0;
	}


	std::cout << "start game" << argv[1] << std::endl;

	DGameServer.start("game", argv[1]);
	
	return 0;
}