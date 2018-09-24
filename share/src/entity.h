
#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "base_util.h"
#include "packet_struct.h"
#include "rpc_wrapper.h"

class entity
{
public:
	entity();
	virtual ~entity();

public:
	virtual bool init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id);

public:
	void call_stub(const std::string& class_name, const std::string& func_name) {
		DRpcWrapper.call_stub(class_name, func_name);
	}

	template <class... Args>
	void call_stub(const std::string& class_name, const std::string& func_name, const Args&... args) {
		DRpcWrapper.call_stub(class_name, func_name, args...);
	}

	void call_entity(const mailbox_info& mailbox, const std::string& func_name) {
		DRpcWrapper.call_entity(mailbox, func_name);
	}

	template <class... Args>
	void call_entity(const mailbox_info& mailbox, const std::string& func_name, const Args&... args) {
		DRpcWrapper.call_entity(mailbox, func_name, args...);
	}

public:
	TServerID_t get_server_id() const;
	TProcessID_t get_game_id() const;
	TEntityID_t get_entity_id() const;
	const mailbox_info& get_mailbox() const;

protected:
	mailbox_info m_mailbox;
};

#endif // !_ENTITY_H_

