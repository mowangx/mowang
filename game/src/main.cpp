
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "log.h"
#include "debug.h"
#include "game_server.h"
#include "socket_manager.h"
#include "time_manager.h"
#include "game_manager_handler.h"
#include "db_manager_handler.h"

void work_run(TProcessID_t process_id)
{
	if (!DGameServer.init(process_id)) {
		log_error("Init game server failed");
		return;
	}
	log_info("Init game server success");
	DGameServer.run();
}

void log_run()
{
	while (true) {
		DLogMgr.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void net_run(TProcessID_t process_id)
{
	if (!DNetMgr.init()) {
		return ;
	}

	TPort_t listen_port = 10200 + process_id;
	if (!DNetMgr.start_listen<db_manager_handler>(listen_port)) {
		return ;
	}

	if (!DNetMgr.start_connect<game_manager_handler>("127.0.0.1", 10000)) {
		return;
	}

	log_info("init socket manager success");

	while (true) {
		DNetMgr.update(0);
		//DNetMgr.test_kick();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	TProcessID_t process_id = atoi(argv[1]);

	std::cout << "start game" << process_id << std::endl;

	std::string module_name = "game";
	DLogMgr.init(module_name + argv[1]);
	gxSetDumpHandler(module_name);

	std::thread log_thread(log_run);
	std::thread net_thread(net_run, std::ref(process_id));
	
	work_run(process_id);

	log_thread.join();
	net_thread.join();
	
	return 0;
}