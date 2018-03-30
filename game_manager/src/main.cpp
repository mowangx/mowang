
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "log.h"
#include "debug.h"
#include "game_manager.h"
#include "server_handler.h"
#include "socket_manager.h"

void work_run()
{
	if (!DGameManager.init()) {
		log_error("Init game manager failed");
		return;
	}
	log_info("Init game manager success");
	DGameManager.run();
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
	if (!DNetMgr.init()) {
		return;
	}

	if (!DNetMgr.start_listen<server_handler>(10000)) {
		return;
	}

	log_info("init socket manager success");

	while (true) {
		DNetMgr.update(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start game manager" << argv[1] << std::endl;

	std::string module_name = "game_manager";
	DLogMgr.init(module_name + argv[1]);
	gxSetDumpHandler(module_name);

	std::thread log_thread(log_run);
	std::thread net_thread(net_run);

	work_run();

	log_thread.join();
	net_thread.join();

	return 0;
}