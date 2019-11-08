#ifndef _GAME_RANDOM_H_
#define _GAME_RANDOM_H_

#include "base_util.h"
#include "singleton.h"

class game_random : public singleton<game_random>
{
public:
	game_random();

public:
	//ReSeed the random number generator
	void reset(TSeedType_t seed);

	//Returns an unsigned integer from 0..RandomMax
	uint32 rand_uint(void);

	//Returns a double in [0.0, 1.0]
	double rand_double();

	bool rand_bool();

	bool rand_odds(uint32 base_num, uint32 rate_num);

	int rand_range(int start, int end);

private:
	TSeedType_t m_seed[3];
};

#define DGameRandom	singleton<game_random>::get_instance()

#endif // !_GAME_RANDOM_H_

