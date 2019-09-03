#include "entity.h"

entity::entity()
{
	m_mailbox.clean_up();
}

entity::~entity()
{
	m_mailbox.clean_up();
}

bool entity::initialize(TEntityID_t entity_id, const TIP_t& ip, TPort_t port)
{
	m_mailbox.entity_id = entity_id;
	m_mailbox.ip = ip;
	m_mailbox.port = port;
	return true;
}

TEntityID_t entity::get_entity_id() const
{
	return m_mailbox.entity_id;
}

const mailbox_info & entity::get_mailbox() const
{
	return m_mailbox;
}
