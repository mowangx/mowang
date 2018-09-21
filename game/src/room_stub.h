#ifndef _ROOM_STUB_H_
#define _ROOM_STUB_H_

#include <unordered_map>

#include "entity.h"
#include "game_struct.h"

class room_stub : public entity
{
public:
	room_stub();
	virtual ~room_stub();

public:
	virtual bool init() override;

public:
	void create_room(TRoleID_t role_id, const dynamic_string& pwd);
	void destroy_room(TRoomID_t room_id);
	void enter_room(TRoleID_t role_id, TRoomID_t room_id, const dynamic_string& pwd);

private:
	std::unordered_map<TRoomID_t, mailbox_info> m_room_id_2_mailbox;
};

#endif // !_ROOM_STUB_H_

