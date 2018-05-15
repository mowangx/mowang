
#include "stdio.h"
#include <iostream>

#include "game_manager.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start game manager" << argv[1] << std::endl;

	DGameManager.start("game_manager", argv[1]);

	return 0;
}