
#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "base_util.h"
#include "packet_struct.h"

class entity
{
public:
	entity() {
		m_mailbox.clean_up();
	}

	virtual ~entity() {
		m_mailbox.clean_up();
	}

public:
	virtual bool init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id) {
		m_mailbox.server_id = server_id;
		m_mailbox.game_id = game_id;
		m_mailbox.entity_id = entity_id;
		return true;
	}

public:
	TEntityID_t get_entity_id() const {
		return m_mailbox.entity_id;
	}

	const mailbox_info& get_mailbox() const {
		return m_mailbox;
	}

protected:
	mailbox_info m_mailbox;
};

#endif // !_ENTITY_H_

