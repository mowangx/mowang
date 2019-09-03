
#ifndef _MAILBOX_MANAGER_H_
#define _MAILBOX_MANAGER_H_

#include <unordered_map>

#include "singleton.h"
#include "packet_struct.h"
#include "etcd_struct.h"

class mailbox_manager : public singleton<mailbox_manager>
{
public:
	mailbox_manager();
	~mailbox_manager();

public:
	bool get_mailbox_by_entity(mailbox_info& mailbox, const std::string& entity_name) const;
	bool get_mailbox_list_by_tag(std::vector<mailbox_info>& mailbox_list, const std::string& tag) const;

public:
	void add_entity(const etcd_entity_detail_info& entity_info);
	void del_entity(const game_process_info& process_info);

private:
	void clean_up();

private:
	std::unordered_map<std::string, etcd_entity_detail_info> m_entity_2_mailbox;
	std::unordered_map<std::string, std::vector<etcd_entity_detail_info>> m_tag_2_mailbox;
};

#define DMailboxMgr singleton<mailbox_manager>::get_instance()

#endif // !_MAILBOX_MANAGER_H_

