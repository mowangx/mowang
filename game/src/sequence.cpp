
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

TSequenceID_t sequence::gen_sequence_id(sequence_type sequence_type)
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
	DGameServer.db_query("sequence", "abc, efg", NULL, [](bool status, const dynamic_string_array2& data) {
		if (status) {
			log_info("query success");
			for (int i = 0; i < data.size(); ++i) {
				const dynamic_string_array& s_data = *(data[i]);
				for (int j = 0; j < s_data.size(); ++j) {
					log_info("query result, %s", s_data[j]->data());
				}
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
