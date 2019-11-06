
#include "server_entity.h"
#include "game_server.h"
#include "entity_manager.h"

server_entity::server_entity()
{
	m_destroy = false;
	m_proxy.clean_up();
}

server_entity::~server_entity()
{
	m_destroy = false;
	m_proxy.clean_up();
}

bool server_entity::init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id)
{
	m_proxy.gate_id = gate_id;
	m_proxy.client_id = client_id;
	return TBaseType_t::initialize(entity_id, DGameServer.get_server_info().ip, DGameServer.get_server_info().port);
}

void server_entity::on_disconnect()
{

}

void server_entity::update_proxy(const proxy_info& proxy)
{
	m_proxy.gate_id = proxy.gate_id;
	m_proxy.client_id = proxy.client_id;
}

void server_entity::destroy()
{
	m_destroy = true;
	DEntityMgr.destroy_entity(get_entity_id());
}

bool server_entity::is_destroy() const
{
	return m_destroy;
}

TProcessID_t server_entity::get_gate_id() const
{
	return m_proxy.gate_id;
}

TSocketIndex_t server_entity::get_client_id() const
{
	return m_proxy.client_id;
}

const proxy_info & server_entity::get_proxy() const
{
	return m_proxy;
}
