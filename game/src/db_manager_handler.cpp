
#include "db_manager_handler.h"
#include "log.h"
#include "game_server.h"
#include "socket.h"

CDbManagerHandler::CDbManagerHandler() : CPacketHandler<CDbManagerHandler>()
{

}

CDbManagerHandler::~CDbManagerHandler()
{

}

void CDbManagerHandler::Setup()
{
	register_handler(12345678, (TPacketHandler)&CDbManagerHandler::handle_test_1);
	register_handler(8888888, (TPacketHandler)&CDbManagerHandler::handle_test_2);
}

TPacketInfo_t* CDbManagerHandler::create_packet_info()
{
	return DGameSerger.allocate_packet_info();
}

char* CDbManagerHandler::create_packet(int n)
{
	return DGameSerger.allocate_memory(n);
}

void CDbManagerHandler::write_packet(TPacketInfo_t* packet_info)
{
	DGameSerger.push_write_packets(packet_info);
}

void CDbManagerHandler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool CDbManagerHandler::handle_test_1(CBasePacket* packet)
{
	CLoginRequest* login = (CLoginRequest*)packet;
	log_info("'%"I64_FMT"u', %d, %d", get_socket_index(), login->get_packet_len(), login->m_user);

	CLogoutRequest logout_ret;
	logout_ret.m_id = 8888888;
	logout_ret.m_len = sizeof(CLogoutRequest);
	logout_ret.m_check = 6666666;
	strcpy(logout_ret.m_name, "Hello World");
	logout_ret.m_user = 168;
	send_packet(logout_ret);
	return true;
}

bool CDbManagerHandler::handle_test_2(CBasePacket* packet)
{
	CLogoutRequest* logout = (CLogoutRequest*)packet;
	log_info("'%"I64_FMT"u', %d, %d, %s", get_socket_index(), logout->get_packet_len(), logout->m_user, logout->m_name);

	CLoginRequest login_ret;
	login_ret.m_id = 12345678;
	login_ret.m_len = sizeof(CLoginRequest);
	login_ret.m_check = 987654321;
	login_ret.m_user = 89;
	send_packet(login_ret);
	return true;
}