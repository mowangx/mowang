
#ifndef _RPC_WRAPPER_H_
#define _RPC_WRAPPER_H_

#include <map>

#include "singleton.h"
#include "base_packet.h"
#include "server_manager.h"
#include "rpc_client.h"

class rpc_wrapper : public singleton<rpc_wrapper>
{
	struct rpc_client_wrapper_info
	{
		TProcessID_t process_id;
		rpc_client* rpc;
		rpc_client_wrapper_info() {
			process_id = INVALID_PROCESS_ID;
			rpc = NULL;
		}
	};

public:
	rpc_wrapper();
	~rpc_wrapper();

public:
	template <typename F, typename... T>
	void call_client(const proxy_info& proxy, const std::string& func_name, std::tuple<T...>& args) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, args);
		}
	}

	template <class T1>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1);
		}
	}

	template <class T1, class T2>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2);
		}
	}

	template <class T1, class T2, class T3>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2, p3);
		}
	}

	template <class T1, class T2, class T3, class T4>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2, p3, p4);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2, p3, p4, p5);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2, p3, p4, p5, p6);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6, const T7& p7) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2, p3, p4, p5, p6, p7);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_client(const proxy_info& proxy, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		rpc_client* rpc = get_client(proxy.server_id, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, p1, p2, p3, p4, p5, p6, p7, p8);
		}
	}

public:
	void call_role(const mailbox_info& mailbox, const std::string& func_name) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name);
		}
	}

	template <class T1>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1);
		}
	}

	template <class T1, class T2>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2);
		}
	}

	template <class T1, class T2, class T3>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2, p3);
		}
	}

	template <class T1, class T2, class T3, class T4>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2, p3, p4);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, 
		const T5& p5) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2, p3, p4, p5);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, 
		const T5& p5, const T6& p6) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2, p3, p4, p5, p6);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6, const T7& p7) {
		rpc_client = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2, p3, p4, p5, p6, p7);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.role_id, func_name, p1, p2, p3, p4, p5, p6, p7, p8);
		}
	}

public:
	template <typename F, typename... T>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name);
		}
	}

	template <class T1>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1);
		}
	}

	template <class T1, class T2>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2);
		}
	}

	template <class T1, class T2, class T3>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2,
		const T3& p3) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2, p3);
		}
	}

	template <class T1, class T2, class T3, class T4>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2,
		const T3& p3, const T4& p4) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2, p3, p4);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2,
		const T3& p3, const T4& p4, const T5& p5) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2, p3, p4, p5);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2,
		const T3& p3, const T4& p4, const T5& p5, const T6& p6) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2, p3, p4, p5, p6);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2,
		const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2, p3, p4, p5, p6, p7);
		}
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_stub(const mailbox_info& mailbox, const std::string& class_name, const std::string& func_name, const T1& p1, const T2& p2,
		const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		rpc_client* rpc = get_random_client(mailbox.server_id);
		if (NULL != rpc) {
			rpc->call_stub(mailbox.server_id, mailbox.game_id, class_name, func_name, p1, p2, p3, p4, p5, p6, p7, p8);
		}
	}

public:
	void register_handle_info(rpc_client* client, server_info_packet* packet);

public:
	TSocketIndex_t get_socket_index(TServerID_t server_id, TProcessID_t process_id) const;

private:
	rpc_client* get_client(TServerID_t server_id, TProcessID_t process_id) const;
	rpc_client* get_random_client(TServerID_t server_id) const;
	uint32 get_client_key_id(TServerID_t server_id, TProcessID_t process_id) const;

private:
	std::map<uint32, rpc_client*> m_clients;
	std::map<uint32, std::vector<rpc_client_wrapper_info*>> m_client_wrappers;
	server_manager m_gates;
};

#define DRpcWrapper singleton<rpc_wrapper>::get_instance()

#endif // !_RPC_WRAPPER_H_
