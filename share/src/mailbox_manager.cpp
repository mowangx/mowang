
#include "mailbox_manager.h"

mailbox_manager::mailbox_manager()
{
	clean_up();
}

mailbox_manager::~mailbox_manager()
{
	clean_up();
}

bool mailbox_manager::get_mailbox_by_entity(mailbox_info& mailbox, const std::string& entity_name) const
{
	auto itr = m_entity_2_mailbox.find(entity_name);
	if (itr == m_entity_2_mailbox.end()) {
		return false;
	}
	const etcd_entity_detail_info& entity_info = itr->second;
	mailbox.entity_id = entity_info.entity_info.entity_id;
	mailbox.port = entity_info.server_info.port;
	memcpy(mailbox.ip.data(), entity_info.server_info.ip.data(), IP_LEN);
	return true;
}

bool mailbox_manager::get_mailbox_list_by_tag(std::vector<mailbox_info>& mailbox_list, const std::string& tag) const
{
	auto itr = m_tag_2_mailbox.find(tag);
	if (itr == m_tag_2_mailbox.end()) {
		return false;
	}
	const std::vector<etcd_entity_detail_info>& entities = itr->second;
	for (auto entity_info : entities) {
		mailbox_info mailbox;
		mailbox.entity_id = entity_info.entity_info.entity_id;
		mailbox.port = entity_info.server_info.port;
		memcpy(mailbox.ip.data(), entity_info.server_info.ip.data(), IP_LEN);
	}
	return true;
}

void mailbox_manager::add_entity(const etcd_entity_detail_info& entity_info)
{
	std::string entity_name = entity_info.entity_info.entity_name.data();
	m_entity_2_mailbox[entity_name] = entity_info;
	std::string tag = entity_info.entity_info.tag.data();
	auto itr = m_tag_2_mailbox.find(tag);
	if (itr == m_tag_2_mailbox.end()) {
		std::vector<etcd_entity_detail_info> entities;
		entities.push_back(entity_info);
		m_tag_2_mailbox[tag] = entities;
	}
	else {
		itr->second.push_back(entity_info);
	}
	
}

void mailbox_manager::del_entity(const game_process_info& process_info)
{
	std::vector<TEntityName_t> entity_names;
	for (auto itr = m_entity_2_mailbox.begin(); itr != m_entity_2_mailbox.end(); ++itr) {
		const etcd_entity_detail_info& entity_info = itr->second;
		if (entity_info.server_info.process_info == process_info) {
			entity_names.push_back(entity_info.entity_info.entity_name);
		}
	}
	for (auto entity_name : entity_names) {
		auto itr = m_entity_2_mailbox.find(std::string(entity_name.data()));
		if (itr != m_entity_2_mailbox.end()) {
			m_entity_2_mailbox.erase(itr);
		}
	}

	for (auto itr = m_tag_2_mailbox.begin(); itr != m_tag_2_mailbox.end(); ++itr) {
		std::vector<etcd_entity_detail_info>& entities = itr->second;
		for (auto it = entities.begin(); it != entities.end(); ) {
			const etcd_entity_detail_info& entity_info = *it;
			if (entity_info.server_info.process_info == process_info) {
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}
	}
}

void mailbox_manager::clean_up()
{
	m_entity_2_mailbox.clear();
	m_tag_2_mailbox.clear();
}
