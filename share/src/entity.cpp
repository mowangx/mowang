#include "entity.h"

entity::entity()
{
	m_mailbox.clean_up();
}

entity::~entity()
{
	m_mailbox.clean_up();
}

bool entity::init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id)
{
	m_mailbox.server_id = server_id;
	m_mailbox.game_id = game_id;
	m_mailbox.entity_id = entity_id;
	return true;
}

TServerID_t entity::get_server_id() const
{
	return m_mailbox.server_id;
}

TProcessID_t entity::get_game_id() const
{
	return m_mailbox.game_id;
}

TEntityID_t entity::get_entity_id() const
{
	return m_mailbox.entity_id;
}

const mailbox_info & entity::get_mailbox() const
{
	return m_mailbox;
}
