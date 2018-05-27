
#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include "singleton.h"
#include "game_enum.h"
#include "base_util.h"
#include "dynamic_array.h"

class sequence : public singleton<sequence>
{
public:
	sequence();
	~sequence();

public:
	void init(const dynamic_array<TSequenceID_t>& data);

public:
	TSequenceID_t gen_sequence_id(sequence_type sequence_type);

public:
	void save();
	void clean_up();

private:
	TSequenceID_t m_sequence_id[MAX_SEQUENCE_TYPE_NUM];
};

#define DSequence singleton<sequence>::get_instance()

#endif // !_SEQUENCE_H_
