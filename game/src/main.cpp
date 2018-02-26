
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "log.h"
#include "debug.h"
#include "game_server.h"

void work_run()
{
	CGameServer gameserver;
	if (!gameserver.init()) {
		log_error("Init game server failed");
		return;
	}
	log_info("Init game server success");
	gameserver.run();
}

void log_run()
{
	while (true) {
		DLogMgr.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void net_run()
{
	while (true) {
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}
	
	std::string module_name = "game";
	DLogMgr.init(module_name + argv[1]);
	gxSetDumpHandler(module_name);

	std::thread log_thread(log_run);
	std::thread net_thread(net_run);
	
	work_run();

	log_thread.join();
	net_thread.join();
	
	return 0;
}