
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "log.h"
#include "debug.h"
#include "gate_server.h"
#include "socket_manager.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "client_handler.h"

void work_run(TProcessID_t process_id)
{
	if (!DGateServer.init(process_id)) {
		log_error("Init game manager failed");
		return;
	}
	log_info("Init game manager success");
	DGateServer.run();
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
	TPort_t listen_port = 10300 + process_id;
	if (!DNetMgr.start_listen<client_handler>(listen_port)) {
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

	TProcessID_t process_id = atoi(argv[1]);

	std::cout << "start gate" << process_id << std::endl;

	std::string module_name = "gate";
	DLogMgr.init(module_name + argv[1]);
	gxSetDumpHandler(module_name);

	if (!DNetMgr.init()) {
		log_error("init socket manager failed");
		return 0;
	}

	std::thread log_thread(log_run);
	std::thread net_thread(net_run, std::ref(process_id));

	work_run(process_id);

	log_thread.join();
	net_thread.join();

	return 0;
}