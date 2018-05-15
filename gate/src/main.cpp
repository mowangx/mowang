
#include "stdio.h"
#include <iostream>

#include "gate_server.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start gate" << argv[1] << std::endl;

	DGateServer.start("gate", argv[1]);

	return 0;
}