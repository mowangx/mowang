
#include "stdio.h"
#include <iostream>

#include "db_server.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start db manager" << argv[1] << std::endl;

	DDbServer.start("db_manager", argv[1]);

	return 0;
}