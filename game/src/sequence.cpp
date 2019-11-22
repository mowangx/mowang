
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

void sequence::init()
{
	DGameServer.db_query(
		"sequence", 
		"sequence_type, sequence_id", 
		gx_to_string("process_id = %d", DGameServer.get_server_info().process_info.process_id).c_str(), 
		[&](bool status, const binary_data& result) {
		if (!status) {
			log_info("query sequence failed");
			return;
		}
		if (result.empty()) {
			log_info("query sequence success but result empty!");
			return;
		}
		int buffer_index = 0;
		uint16 row_count = 0;
		rpc_param_parse<uint16, uint16>::parse_param(row_count, result.data(), buffer_index);
		for (int i = 0; i < row_count; ++i) {
			uint8 sequence_type = 0;
			rpc_param_parse<uint8, uint8>::parse_param(sequence_type, result.data(), buffer_index);
			rpc_param_parse<TSequenceID_t, TSequenceID_t>::parse_param(m_sequence_id[sequence_type], result.data(), buffer_index);
			log_info("load sequence! %d, %d", sequence_type, m_sequence_id[sequence_type]);
		}
	});
}

TSequenceID_t sequence::gen_sequence_id(sequence_type sequence_type)
{
	++m_sequence_id[sequence_type];
	save(sequence_type);
	return m_sequence_id[sequence_type];
}

void sequence::save(sequence_type sequence_type)
{
	DGameServer.db_insert(
		"sequence", 
		"(sequence_type, process_id, sequence_id)", 
		gx_to_string("(%d, %d, %d) on duplicate key update sequence_id = values(sequence_id)", 
			sequence_type, DGameServer.get_server_info().process_info.process_id, m_sequence_id[sequence_type]).c_str(),
		[](bool status) {
		if (status) {
			log_info("save sequence success");
		}
		else {
			log_error("save sequence failed");
		}
	});
}

void sequence::clean_up()
{
	for (int i = 0; i < MAX_SEQUENCE_TYPE_NUM; ++i) {
		m_sequence_id[i] = INVALID_SEQUENCE_ID;
	}
}
