
#ifndef _WS_SERVICE_H_
#define _WS_SERVICE_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "service.h"

class ws_service : public service
{
	typedef service TBaseType_t;
public:
	ws_service(game_process_type process_type);
	virtual ~ws_service();

public:
	virtual bool init(TProcessID_t process_id) override;
	virtual void init_ws_process_func();
	virtual void init_threads() override;
	virtual void ws_run();

protected:
	virtual void do_loop(TGameTime_t diff) override;
	virtual void do_ws_loop(TGameTime_t diff);

public:
	virtual void push_ws_write_packets(TSocketIndex_t socket_index, const std::string& packet);

protected:
	virtual void process_ws_init_sockets(std::vector<web_socket_wrapper_base*>& sockets);
	virtual void process_ws_close_sockets(std::vector<web_socket_wrapper_base*>& sockets);
	virtual void process_ws_packets(std::vector<ws_packet_recv_info*>& packets);

protected:
	std::vector<packet_send_info*> m_write_ws_packets;
	std::unordered_map<std::string, std::function<void(TSocketIndex_t, boost::property_tree::ptree*)>> m_cmd_2_parse_func;
};

#endif // !_WS_SERVICE_H_

