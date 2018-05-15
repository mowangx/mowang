
#include "sequence.h"
#include "game_server.h"

sequence::sequence()
{
	clean_up();
}

sequence::~sequence()
{
	clean_up();
}

void sequence::init(const dynamic_array<TSequenceID_t>& data)
{
	for (int i = 0; i < data.size(); ++i) {
		m_sequence_id[i] = data[i];
	}
}

TSequenceID_t sequence::gen_sequence_id(uint8 sequence_type)
{
	++m_sequence_id[sequence_type];
	save();
	return m_sequence_id[sequence_type];
}

void sequence::save()
{
	//DGameServer.db_insert("sequence", "role_index = 1", [](bool status) {
	//	if (status) {
	//		log_info("save success");
	//	}
	//	else {
	//		log_info("save failed");
	//	}
	//});
	DGameServer.db_query("sequence", "abc, efg", NULL, [](bool status, const dynamic_string_array& data) {
		if (status) {
			log_info("query success");
			for (int i = 0; i < data.size(); ++i) {
				log_info("query result, %s", data[i]->data());
			}
		}
		else {
			log_info("query failed");
		}
	});
}

void sequence::clean_up()
{
	for (int i = 0; i < MAX_SEQUENCE_TYPE_NUM; ++i) {
		m_sequence_id[i] = INVALID_SEQUENCE_ID;
	}
}
