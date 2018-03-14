
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>

#include "log.h"
#include "debug.h"
#include "socket_manager.h"

#include "db_server.h"

void work_run()
{
	CDbServer db_server;
	if (!db_server.init()) {
		return;
	}

	db_server.run();
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
	log_info("init socket manager success");

	while (true) {
		DNetMgr.update(0);
		if (DNetMgr.socket_num() < 1000) {
			if (!DNetMgr.start_connect("127.0.0.1", 10000)) {
				log_info("connect failed");
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start db manager" << argv[1] << std::endl;

	std::string module_name = "db_manager";
	DLogMgr.init(module_name + argv[1]);
	gxSetDumpHandler(module_name);

	std::thread log_thread(log_run);
	std::thread net_thread(net_run);

	work_run();

	log_thread.join();
	net_thread.join();

	return 0;
}