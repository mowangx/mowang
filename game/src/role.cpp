
#include "role.h"
#include <algorithm>

#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"
#include "sequence.h"
#include "entity_manager.h"
#include "game_enum.h"
#include "timer.h"
#include "error_code.h"
#include "binary_string.h"
#include "tbl_building.h"
#include "tbl_building_upgrade.h"

role::role() : server_entity()
{
	clean_up();
}

role::~role()
{
	clean_up();
}

bool role::init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id)
{
	if (!TBaseType_t::init(entity_id, gate_id, client_id)) {
		return false;
	}
	DRegisterEntityRpc(get_entity_id(), this, role, on_relay_ready, 1);
	DRegisterEntityRpc(get_entity_id(), this, role, on_relay_login, 1);
	DRegisterEntityRpc(get_entity_id(), this, role, on_register_role, 1);
	DRegisterEntityRpc(get_entity_id(), this, role, kick_role, 0);
	DRegisterEntityRpc(get_entity_id(), this, role, disconnect_client, 0);
	DRegisterEntityRpc(get_entity_id(), this, role, income_resource, 1);
	DRegisterEntityRpc(get_entity_id(), this, role, build_building, 2);
	DRegisterEntityRpc(get_entity_id(), this, role, up_building_level, 1);
	return true;
}

void role::on_relay_ready(const mailbox_info& mailbox)
{
	log_info("kick current client by relay login! role id %" I64_FMT "u", get_role_id());
	m_status = ROLE_STATUS_RELAY;
	DRpcWrapper.call_entity(mailbox, "on_wait_login", get_mailbox());
	std::string msg = gx_to_string("{\"cmd\": \"kick\"}");
	call_ws_client(msg);
	DGameServer.remove_client_id(get_client_id());
}

void role::on_relay_login(const proxy_info& proxy)
{
	log_info("migrate finish, on relay login success! role id %" I64_FMT "u", get_role_id());
	m_status = ROLE_STATUS_NORMAL;
	update_proxy(proxy);
	DRpcWrapper.call_stub("roll_stub", "update_by_relay", get_account_id(), get_role_id(), get_mailbox());
	on_login_success();
	DGameServer.update_client_entity_id(get_client_id(), get_entity_id());
	rpc_client* rpc = DRpcWrapper.get_client_by_process_id(PROCESS_GATE, get_gate_id());
	if (nullptr == rpc) {
		log_error("can not find gate proxy! gate id %u, role id %" I64_FMT "u", get_gate_id(), get_role_id());
		return;
	}
	rpc->call_remote_func("update_client_process_info", DGameServer.get_server_info().process_info);
}

void role::register_role()
{
	log_info("role login success, start register role! role id %" I64_FMT "u", get_role_id());
	DRpcWrapper.call_stub("roll_stub", "register_role", get_account_id(), get_role_id(), get_mailbox());
}

void role::on_register_role(bool status)
{
	if (status) {
		if (m_status == ROLE_STATUS_DISCONNECT) {
			log_info("on register role success, but disconnect client, start logout! role id %" I64_FMT "u ", get_role_id());
			logout(true);
		}
		else {
			log_info("on register role success! role id %" I64_FMT "u ", get_role_id());
			m_status = ROLE_STATUS_NORMAL;
			on_login_success();
		}
	}
	else {
		log_info("on register role failed, kick current role! role id %" I64_FMT "u ", get_role_id());
		std::string msg = gx_to_string("{\"cmd\": \"login\", \"ret_code\": %d}", ERR_LOGIN_FAILED_BY_RELAY);
		call_ws_client(msg);
		logout(false);
	}
}

void role::kick_role()
{
	log_info("kick role! role id %" I64_FMT "u", get_role_id());
	std::string msg = gx_to_string("{\"cmd\": \"kick\"}");
	call_ws_client(msg);
	logout(true);
}

void role::disconnect_client()
{
	log_info("role disconnect client! role id %" I64_FMT "u", get_role_id());
	if (is_destroy()) {
		return;
	}
	if (m_status == ROLE_STATUS_NORMAL) {
		logout(true);
	}
	else {
		m_status = ROLE_STATUS_DISCONNECT;
		DTimer.add_timer(5, false, nullptr, [&](void* param, TTimerID_t timer_id) {
			if (m_status != ROLE_STATUS_NORMAL) {
				logout(true);
			}
			log_info("role relay timeout! role id %" I64_FMT "u", get_role_id());
		});
	}
}

void role::on_login_success()
{
	std::string msg = gx_to_string("{\"cmd\": \"login\", \"ret_code\": %d, \"account_id\": %" I64_FMT "u, \"role_id\": %" I64_FMT "u, \"level\": %d, \"sex\": %d, \"role_name\": \"%s\"",
		ERR_SUCCESS, m_role_data.account_id, m_role_data.role_id, m_role_data.level, m_role_data.sex, m_role_data.role_name.data());
	std::string buildings = ",\"buildings\": [";
	for (int i = 0; i < m_role_data.buildings.size(); ++i) {
		if (i > 0) {
			buildings += ", ";
		}
		const building_info& building_data = m_role_data.buildings[i];
		resource_info resource_speed;
		get_resource_speed(resource_speed, building_data.building_type, building_data.building_level);
		buildings += gx_to_string("{\"index\": %d, \"type\": %d, \"level\": %d, \"resource_type\": %d, \"resource_num\": %d, \"speed\": %d, \"start_time\": %u, \"income_time\": %u}", 
			building_data.building_index, building_data.building_type, building_data.building_level, building_data.resource_data.resource_type, 
			building_data.resource_data.resource_num, resource_speed.resource_num, building_data.start_time, building_data.income_time);
	}
	msg = msg + buildings + "]";

	std::string resources = ",\"resources\": [";
	for (int i = 0; i < m_role_data.resources.size(); ++i) {
		if (i > 0) {
			resources += ", ";
		}
		const resource_info resource_data = m_role_data.resources[i];
		resources += gx_to_string("{\"type\": %d, \"num\": %d}", resource_data.resource_type, resource_data.resource_num);
	}
	msg = msg + resources + "]}";
	call_ws_client(msg);
}

void role::notify_update_resource()
{
	std::string msg = "{\"cmd\": \"update_resource\",\"resources\": [";
	for (int i = 0; i < m_role_data.resources.size(); ++i) {
		if (i > 0) {
			msg += ", ";
		}
		const resource_info resource_data = m_role_data.resources[i];
		msg += gx_to_string("{\"type\": %d, \"num\": %d}", resource_data.resource_type, resource_data.resource_num);
	}
	msg += "]}";
	call_ws_client(msg);
}

void role::income_resource(TBuildingIndex_t index)
{
	int ret_code = check_income_resource(index);
	std::string msg = gx_to_string("{\"cmd\": \"income_resource\", \"ret_code\": %d, \"index\": %d}", ret_code, index);
	call_ws_client(msg);
	if (ret_code == ERR_SUCCESS) {
		on_income_resource(index);
	}
}

int role::check_income_resource(TBuildingIndex_t index)
{
	auto itr = std::find(m_role_data.buildings.begin(), m_role_data.buildings.end(), index);
	if (itr == m_role_data.buildings.end()) {
		return ERR_BUILDING_NOT_EXIST;
	}
	const building_info& building_data = *itr;
	tbl_building_upgrade_config* building_config = DTblBuildingUpgradeMgr.get(building_data.building_level);
	auto income_itr = building_config->income_interval.find(building_data.building_type);
	if (income_itr == building_config->income_interval.end()) {
		log_error("can not find income interval from building upgrade config! building level %d, building type %d", building_data.building_level, building_data.building_type);
		return ERR_FAILED;
	}
	if ((building_data.income_time + income_itr->second) > DTimeMgr.now_sys_time()) {
		return ERR_BUILDING_INCOME_TOO_SHORT;
	}
	return ERR_SUCCESS;
}

void role::on_income_resource(TBuildingIndex_t index)
{
	auto itr = std::find(m_role_data.buildings.begin(), m_role_data.buildings.end(), index);
	building_info& building_data = *itr;
	on_building_changed(building_data);
	auto resource_itr = std::find(m_role_data.resources.begin(), m_role_data.resources.end(), building_data.resource_data.resource_type);
	if (resource_itr == m_role_data.resources.end()) {
		resource_info resource_data;
		resource_data.resource_type = building_data.resource_data.resource_type;
		resource_data.resource_num = building_data.resource_data.resource_num;
	}
	else {
		resource_info& resource_data = *resource_itr;
		resource_data.resource_num += building_data.resource_data.resource_num;
	}
	building_data.resource_data.resource_num = 0;
	building_data.income_time = DTimeMgr.now_sys_time();
	notify_update_building(building_data);
	notify_update_resource();
}

void role::build_building(TBuildingIndex_t index, TBuildingType_t building_type)
{
	int ret_code = check_build_building(index, building_type);
	std::string msg = gx_to_string("{\"cmd\": \"build_building\", \"ret_code\": %d, \"index\": %d, \"type\": %d}", ret_code, index, building_type);
	call_ws_client(msg);
	if (ret_code == ERR_SUCCESS) {
		on_build_building_success(index, building_type);
	}
}

int role::check_build_building(TBuildingIndex_t index, TBuildingType_t building_type)
{
	tbl_building_config* building_config = DTblBuildingMgr.get(building_type);
	if (nullptr == building_config) {
		return ERR_BUILDING_TYPE_NOT_EXIST;
	}
	for (auto building_data : m_role_data.buildings) {
		if (building_data.building_index == index) {
			return ERR_BUILDING_SAME_INDEX;
		}
	}
	const std::vector<TResourceNum_t>& resources = building_config->unlock_need_money;
	for (int i = 0; i < resources.size(); i += 2) {
		if (!check_resource(resources[i], resources[i + 1])) {
			return ERR_BUILDING_LOCK;
		}
	}
	return ERR_SUCCESS;
}

void role::on_build_building_success(TBuildingIndex_t index, TBuildingType_t building_type)
{
	tbl_building_config* building_config = DTblBuildingMgr.get(building_type);
	const std::vector<TResourceNum_t>& resources = building_config->unlock_need_money;
	for (int i = 0; i < resources.size(); i += 2) {
		desc_resource(resources[i], resources[i + 1]);
	}
	building_info building_data;
	building_data.building_index = index;
	building_data.building_type = building_type;
	building_data.building_level = 1;
	building_data.start_time = DTimeMgr.now_sys_time();
	on_building_changed(building_data);
	m_role_data.buildings.push_back(building_data);
	notify_update_building(building_data);
	notify_update_resource();
}

void role::up_building_level(TBuildingIndex_t index)
{
	int ret_code = check_up_building_level(index);
	std::string msg = gx_to_string("{\"cmd\": \"up_building_level\", \"ret_code\": %d, \"index\": %d}",
		ret_code, index);
	call_ws_client(msg);
	if (ret_code == ERR_SUCCESS) {
		on_up_building_level_success(index);
	}
}

int role::check_up_building_level(TBuildingIndex_t index)
{
	auto itr = std::find(m_role_data.buildings.begin(), m_role_data.buildings.end(), index);
	if (itr == m_role_data.buildings.end()) {
		return ERR_BUILDING_NOT_EXIST;
	}
	building_info& building_data = *itr;
	tbl_building_upgrade_config* building_config = DTblBuildingUpgradeMgr.get(building_data.building_level);
	if (nullptr == building_config) {
		return ERR_FAILED;
	}
	auto building_itr = building_config->upgrade_need_money.find(building_data.building_type);
	if (building_itr == building_config->upgrade_need_money.end()) {
		return ERR_FAILED;
	}
	const std::vector<TResourceNum_t>& resources = building_itr->second;
	for (int i = 0; i < resources.size(); i += 2) {
		if (!check_resource(resources[i], resources[i + 1])) {
			return ERR_BUILDING_RESOURCE_NOT_ENOUGH;
		}
	}
	return ERR_SUCCESS;
}

void role::on_up_building_level_success(TBuildingIndex_t index)
{
	auto itr = std::find(m_role_data.buildings.begin(), m_role_data.buildings.end(), index);
	building_info& building_data = *itr;
	tbl_building_upgrade_config* building_config = DTblBuildingUpgradeMgr.get(building_data.building_level);
	auto building_itr = building_config->upgrade_need_money.find(building_data.building_type);
	const std::vector<TResourceNum_t>& resources = building_itr->second;
	for (int i = 0; i < resources.size(); i += 2) {
		TResourceType_t resource_type = resources[i];
		desc_resource(resources[i], resources[i + 1]);
	}
	on_building_changed(building_data);
	building_data.building_level += 1;
	notify_update_building(building_data);
	notify_update_resource();
}

bool role::check_resource(TResourceType_t resource_type, TResourceNum_t resource_num)
{
	auto itr = std::find(m_role_data.resources.begin(), m_role_data.resources.end(), resource_type);
	if (itr == m_role_data.resources.end()) {
		return false;
	}
	resource_info& resource_data = *itr;
	if (resource_data.resource_num < resource_num) {
		return false;
	}
	return true;
}

void role::desc_resource(TResourceType_t resource_type, TResourceNum_t resource_num)
{
	auto itr = std::find(m_role_data.resources.begin(), m_role_data.resources.end(), resource_type);
	if (itr == m_role_data.resources.end()) {
		return ;
	}
	resource_info& resource_data = *itr;
	resource_data.resource_num -= resource_num;
}

void role::on_building_changed(building_info& building_data)
{
	resource_info resource_speed;
	get_resource_speed(resource_speed, building_data.building_type, building_data.building_level);
	TGameTime_t now = DTimeMgr.now_sys_time();
	building_data.resource_data.resource_type = resource_speed.resource_type;
	building_data.resource_data.resource_num = building_data.resource_data.resource_num + (now - building_data.start_time) * resource_speed.resource_num;
	building_data.start_time = now;
}

void role::get_resource_speed(resource_info& resource_data, TBuildingType_t building_type, TLevel_t level) const
{
	tbl_building_upgrade_config* building_config = DTblBuildingUpgradeMgr.get(level);
	if (nullptr == building_config) {
		return ;
	}
	auto itr = building_config->product.find(building_type);
	if (itr == building_config->product.end()) {
		return ;
	}
	const std::vector<TResourceNum_t>& resources = itr->second;
	if (resources.size() < 2) {
		return;
	}
	resource_data.resource_type = resources[0];
	resource_data.resource_num = resources[1];
}

void role::notify_update_building(const building_info & building_data)
{
	resource_info resource_speed;
	get_resource_speed(resource_speed, building_data.building_type, building_data.building_level);
	std::string msg = gx_to_string("{\"cmd\": \"update_building\", \"index\": %d, \"type\": %d, \"level\": %d, \"resource_type\": %d, \"resource_num\": %d, \"speed\": %d, \"start_time\": %u, \"income_time\": %u}",
		building_data.building_index, building_data.building_type, building_data.building_level, building_data.resource_data.resource_type, building_data.resource_data.resource_num,
		resource_speed.resource_num, building_data.start_time, building_data.income_time);
	call_ws_client(msg);
}

TLevel_t role::get_level() const
{
	return m_role_data.level;
}

void role::set_level(TLevel_t lvl)
{
	m_role_data.level = lvl;
}

void role::add_level(TLevel_t lvl)
{
	m_role_data.level += lvl;
}

void role::set_role_data(const role_info & role_data)
{
	m_role_data = role_data;
}

const TRoleName_t & role::get_role_name() const
{
	return m_role_data.role_name;
}

TAccountID_t role::get_account_id() const
{
	return m_role_data.account_id;
}

TRoleID_t role::get_role_id() const
{
	return m_role_data.role_id;
}

void role::save()
{
	uint16 building_size = sizeof(building_info) * m_role_data.buildings.size();
	dynamic_string* building = allocate_binary_string(building_size);
	dynamic_struct_2_bstr<building_info>(building->data(), m_role_data.buildings);
	DGameServer.db_update(
		"role",
		gx_to_string("set level = %d, buildings = '%s'", get_level(), building->data()).c_str(),
		gx_to_string("role_id = '%" I64_FMT "u'", get_role_id()).c_str(),
		[&](bool status) {
		if (status) {
			log_info("save role success!");
		}
	});
}

void role::logout(bool need_unregister)
{
	//log_info("logout role! role id %" I64_FMT "u", get_role_id());
	//if (need_unregister) {
	//	save();
	//	DRpcWrapper.call_stub("roll_stub", "unregister_role", get_account_id(), get_role_id());
	//}
	//destroy();
}

void role::clean_up()
{
	m_status = INVALID_ROLE_STATUS;
	m_role_data.clean_up();
}