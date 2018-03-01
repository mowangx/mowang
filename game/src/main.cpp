
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "log.h"
#include "debug.h"
#include "game_server.h"
#include "socket_manager.h"

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
		//std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void net_run()
{
	CSocketManager* net = new CSocketManager();
	if (NULL == net) {
		return ;
	}

	if (!net->init()) {
		return ;
	}

	if (!net->start_listen(10000)) {
		return ;
	}

	log_info("init socket manager success");

	while (true) {
		net->update(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start game" << argv[1] << std::endl;
	
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