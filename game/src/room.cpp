
#include "room.h"
#include "game_enum.h"
#include "string_common.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

room::room()
{
	m_room_id = INVALID_ROOM_ID;
	m_role_id_2_role_info.clear();
}

room::~room()
{
	m_role_id_2_role_info.clear();
}

bool room::init()
{
	log_info("room init");
	DRegisterStubRpc(this, room, enter_room, 3);
	DRegisterStubRpc(this, room, leave_room, 1);
	DRegisterStubRpc(this, room, ready_start, 1);
	DRegisterStubRpc(this, room, pop_cards, 2);
	return true;
}

void room::enter_room(TRoleID_t role_id, const mailbox_info & mailbox, const proxy_info& proxy)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		room_role_info role_info;
		role_info.status = false;
		role_info.mailbox = mailbox;
		role_info.proxy = proxy;
		m_role_id_2_role_info[role_id] = role_info;
	} 
	else { 
		room_role_info& role_info = itr->second;
		role_info.mailbox = mailbox;
		role_info.proxy = proxy;
	}
}

void room::leave_room(TRoleID_t role_id)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		return;
	}
	m_role_id_2_role_info.erase(itr);

	if (m_role_id_2_role_info.empty()) {
		DRpcWrapper.call_stub("room_stub", "destroy_room", m_room_id);
	}
}

void room::ready_start(TRoleID_t role_id)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		return;
	}
	room_role_info& role_info = itr->second;
	role_info.status = true;

	start_game();
}

void room::pop_cards(TRoleID_t role_id, const dynamic_array<TCardIndex_t>& card_ids)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		return;
	}

	std::vector<TCardIndex_t>& role_cards = itr->second.cards;
	for (int i = 0; i < card_ids.size(); ++i) {
		if (std::find(role_cards.begin(), role_cards.end(), card_ids[i]) == role_cards.end()) {
			return ;
		}
	}

	std::string cards = "";
	for (int i = 0; i < card_ids.size(); ++i) {
		auto itr = std::find(role_cards.begin(), role_cards.end(), card_ids[i]);
		role_cards.erase(itr);
		if (i == 0) {
			cards += gx_to_string("%u", card_ids[i]);
		}
		else {
			cards += gx_to_string(", %u", card_ids[i]);
		}
	}

	std::string msg = gx_to_string("{\"cmd\": \"pop_cards\", \"role_id\": %" I64_FMT "u, \"cards\": [%s]}", 
		role_id, cards.data());

	for (auto itr = m_role_id_2_role_info.begin(); itr != m_role_id_2_role_info.end(); ++itr) {
		if (itr->first == role_id) {
			continue;
		}

		room_role_info& role_info = itr->second;
		DRpcWrapper.call_ws_client(role_info.proxy, msg);
	}

	finish_game(role_id);
}

void room::start_game()
{
	if (m_role_id_2_role_info.size() < 3) {
		return;
	}

	for (auto itr = m_role_id_2_role_info.begin(); itr != m_role_id_2_role_info.end(); ++itr) {
		const room_role_info& role_info = itr->second;
		if (!role_info.status) {
			return;
		}
	}

	for (auto itr = m_role_id_2_role_info.begin(); itr != m_role_id_2_role_info.end(); ++itr) {
		const room_role_info& role_info = itr->second;
		std::string cards = "";
		for (auto itr = role_info.cards.begin(); itr != role_info.cards.end(); ++itr) {
			if (itr == role_info.cards.begin()) {
				cards += gx_to_string("%u", *itr);
			}
			else {
				cards += gx_to_string(", %u", *itr);
			}
		}
		std::string msg = gx_to_string("{\"cmd\": \"add_cards\", \"cards\": [%s]}", cards.data());
		DRpcWrapper.call_ws_client(role_info.proxy, msg);
	}
}

void room::finish_game(TRoleID_t role_id)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		return;
	}

	room_role_info& role_info = itr->second;
	if (role_info.cards.size() > 0) {
		return;
	}

	for (auto itr = m_role_id_2_role_info.begin(); itr != m_role_id_2_role_info.end(); ++itr) {
		const room_role_info& role_info = itr->second;
		std::string cards = "";
		for (auto card_itr = role_info.cards.begin(); card_itr != role_info.cards.end(); ++card_itr) {
			if (card_itr == role_info.cards.begin()) {
				cards += gx_to_string("%u", *card_itr);
			}
			else {
				cards += gx_to_string(", %u", *card_itr);
			}
		}
		std::string msg = gx_to_string("{\"cmd\": \"game_over\", \"result\": %u}", role_id == itr->first ? 1 : 2);
		DRpcWrapper.call_ws_client(role_info.proxy, msg);
	}
}
