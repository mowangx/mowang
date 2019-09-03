
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

#include "db_server.h"

#include "db_packet_handler.h"
#include "tcp_manager.h"
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

	db_packet_handler::Setup();

	DRegisterServerRpc(this, db_server, register_server, 2);
	DRegisterServerRpc(this, db_server, on_register_entities, 5);
	DRegisterServerRpc(this, db_server, on_unregister_process, 4);

	if (!DNetMgr.start_listen<db_packet_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

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


	TBaseType_t::work_run();
}

void db_server::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);
	DExecutorMgr.update(diff);
}

bool db_server::connect_server(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<db_packet_handler>(ip, port);
}
