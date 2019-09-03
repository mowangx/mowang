#ifndef _ROOM_H_
#define _ROOM_H_

#include <map>

#include "server_entity.h"
#include "game_struct.h"

class room : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	room();
	virtual ~room();

public:
	virtual bool init(TEntityID_t entity_id) override;

public:
	void enter_room(TRoleID_t role_id, const mailbox_info& mailbox, const proxy_info& proxy);
	void leave_room(TRoleID_t role_id);

public:
	void set_room_id(TRoomID_t room_id);

public:
	void ready_start(TRoleID_t role_id);
	void pop_cards(TRoleID_t role_id, const dynamic_array<TCardIndex_t>& card_ids);

private:
	void start_game();
	void finish_game(TRoleID_t role_id);
	
private:
	TRoomID_t m_room_id;
	std::map<TRoleID_t, room_role_info> m_role_id_2_role_info;
};

#endif // !_ROOM_H_

