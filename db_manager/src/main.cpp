﻿
#include "stdio.h"
#include <iostream>
#include <thread>
#include <chrono>


#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

#include "log.h"
#include "debug.h"
#include "socket_manager.h"

#include "db_server.h"
#include "game_server_handler.h"
#include "game_manager_handler.h"


void work_run(TProcessID_t process_id)
{
	//mongocxx::instance instance{}; // This should be done only once.
	//mongocxx::uri uri("mongodb://127.0.0.1:27017");
	//mongocxx::client client(uri);
	//mongocxx::database db = client["test"];
	//mongocxx::collection coll = db["role"];
	//auto builder = bsoncxx::builder::stream::document{};
	//bsoncxx::document::value doc_value = builder
	//	<< "name" << "MongoDB"
	//	<< "type" << "database"
	//	<< "count" << 1
	//	<< "versions" << bsoncxx::builder::stream::open_array
	//	<< "v3.2" << "v3.0" << "v2.6"
	//	<< bsoncxx::builder::stream::close_array
	//	<< "info" << bsoncxx::builder::stream::open_document
	//	<< "x" << 203
	//	<< "y" << 102
	//	<< bsoncxx::builder::stream::close_document
	//	<< bsoncxx::builder::stream::finalize;
	//bsoncxx::document::view view = doc_value.view();
	//bsoncxx::document::element element = view["name"];
	//if (element.type() != bsoncxx::type::k_utf8) {
	//	// Error
	//}
	//std::string name = element.get_utf8().value.to_string();
	//try {
	//	bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
	//		coll.insert_one(view);
	//}
	//catch (mongocxx::bulk_write_exception& e) {
	//	std::cout << e.code() << e.what() << std::endl;
	//}

	if (!DDbServer.init(process_id)) {
		return;
	}

	DDbServer.run();
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
		return;
	}
	log_info("init socket manager success");

	TPort_t listen_port = 10100 + process_id;
	if (!DNetMgr.start_listen<game_server_handler>(listen_port)) {
		return;
	}

	if (!DNetMgr.start_connect<game_manager_handler>("127.0.0.1", 10000)) {
		return;
	}

	while (true) {
		DNetMgr.update(0);
		//DNetMgr.test_kick();
		//if (DNetMgr.socket_num() < 1000) {
		//	if (!DNetMgr.start_connect<game_server_handler>("127.0.0.1", 10100)) {
		//		log_info("connect failed");
		//		break;
		//	}
		//}
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

	std::cout << "start db manager" << process_id << std::endl;

	std::string module_name = "db_manager";
	DLogMgr.init(module_name + argv[1]);
	gxSetDumpHandler(module_name);

	std::thread log_thread(log_run);
	std::thread net_thread(net_run, std::ref(process_id));

	work_run(process_id);

	log_thread.join();
	net_thread.join();

	return 0;
}