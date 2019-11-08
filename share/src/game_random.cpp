
#include "game_random.h"

game_random::game_random()
{
}

void game_random::reset(TSeedType_t seed)
{
	m_seed[0] = (seed ^ 0xFEA09B9DLU) & 0xFFFFFFFELU;
	m_seed[0] ^= (((m_seed[0] << 7) & RandomMax) ^ m_seed[0]) >> 31;

	m_seed[1] = (seed ^ 0x9C129511LU) & 0xFFFFFFF8LU;
	m_seed[1] ^= (((m_seed[1] << 2) & RandomMax) ^ m_seed[1]) >> 29;

	m_seed[2] = (seed ^ 0x2512CFB8LU) & 0xFFFFFFF0LU;
	m_seed[2] ^= (((m_seed[2] << 9) & RandomMax) ^ m_seed[2]) >> 28;

	rand_uint();
}

uint32 game_random::rand_uint()
{
	m_seed[0] = (((m_seed[0] & 0xFFFFFFFELU) << 24) & RandomMax)
		^ ((m_seed[0] ^ ((m_seed[0] << 7) & RandomMax)) >> 7);

	m_seed[1] = (((m_seed[1] & 0xFFFFFFF8LU) << 7) & RandomMax)
		^ ((m_seed[1] ^ ((m_seed[1] << 2) & RandomMax)) >> 22);

	m_seed[2] = (((m_seed[2] & 0xFFFFFFF0LU) << 11) & RandomMax)
		^ ((m_seed[2] ^ ((m_seed[2] << 9) & RandomMax)) >> 17);

	return (m_seed[0] ^ m_seed[1] ^ m_seed[2]);
}

double game_random::rand_double()
{
	return static_cast<double>(rand_uint()) / (static_cast<double>(RandomMax));
}

bool game_random::rand_bool()
{
	return rand_uint() > (MAX_UINT32_NUM >> 1);
}

bool game_random::rand_odds(uint32 base_num, uint32 rate_num)
{
	return (rand_uint() % (base_num + 1)) <= rate_num;
}

int game_random::rand_range(int start, int end)
{
	if (start > end) {
		std::swap(start, end);
	}
	return rand_uint() % (end - start + 1) + start;
}
