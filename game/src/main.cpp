
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
	DLogMgr.init("game");
	gxSetDumpHandler("game");
	std::thread log_thread(log_run);
	std::thread net_thread(net_run);
	work_run();
	log_thread.join();
	net_thread.join();
	return 0;
}