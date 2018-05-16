
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

#include "db_server.h"

#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "socket_manager.h"
#include "rpc_proxy.h"
#include "executor_manager.h"

db_server::db_server() : service(PROCESS_DB)
{
	
}

db_server::~db_server()
{
	
}

bool db_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	if (!DExecutorMgr.init()) {
		return false;
	}

	game_server_handler::Setup();
	game_manager_handler::Setup();

	DRegisterServerRpc(this, db_server, register_server, 2);

	return true;
}

void db_server::work_run()
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
	connect_game_manager_loop(m_config.get_game_manager_listen_ip(), m_config.get_game_manager_listen_port());
	TBaseType_t::work_run();
}

void db_server::net_run()
{
	if (!DNetMgr.start_listen<game_server_handler>(m_server_info.port)) {
		return;
	}

	log_info("init socket manager success");

	while (true) {
		DNetMgr.update(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

void db_server::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);
	DExecutorMgr.update(diff);
}

bool db_server::connect_game_manager(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_manager_handler>(ip, port);
}
