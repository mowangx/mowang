#ifndef _ROOM_STUB_H_
#define _ROOM_STUB_H_

#include <unordered_map>

#include "entity.h"
#include "game_struct.h"

class room_stub : public entity
{
	typedef entity TBaseType_t;

public:
	room_stub();
	virtual ~room_stub();

public:
	virtual bool init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id) override;

public:
	void create_room(const mailbox_info& mailbox, const dynamic_string& pwd, const mailbox_info& role_mailbox);
	void destroy_room(TRoomID_t room_id);
	void enter_room(TRoomID_t room_id, TRoleID_t role_id, const mailbox_info& mailbox, const proxy_info& proxy, const dynamic_string& pwd);

private:
	TRoomID_t m_room_id;
	std::unordered_map<TRoomID_t, mailbox_info> m_room_id_2_mailbox;
};

#endif // !_ROOM_STUB_H_

