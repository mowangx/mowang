
#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include "types_def.h"

class CGameServer
{
public:
	CGameServer();
	~CGameServer();

public:
	bool init();
	void run();

private:
};

#endif