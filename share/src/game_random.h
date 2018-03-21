#ifndef _GAME_RANDOM_H_
#define _GAME_RANDOM_H_

#include "base_util.h"
#include "singleton.h"

class CGameRandom : public CSingleton<CGameRandom>
{
public:

	CGameRandom(TSeedType_t seed = 0);

public:
	//ReSeed the random number generator
	void reset(TSeedType_t seed = 0);

	//Returns an unsigned integer from 0..RandomMax
	uint32 rand_uint(void);

	//Returns a double in [0.0, 1.0]
	double rand_double();

	bool rand_bool();

	bool rand_odds(uint32 base_num, uint32 rate_num);

	template<typename T>
	T get_rand(T start, T end) {
		if (start > end) {
			std::swap(start, end);
		}
		return rand_uint() % (end - start + 1) + start;
	}

private:
	TSeedType_t m_seed[3];
};

#define DGameRandom	CSingleton<CGameRandom>::getInstance()

#endif // !_GAME_RANDOM_H_

