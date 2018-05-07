
#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "base_util.h"

class entity
{
public:
	entity() {

	}

	virtual ~entity() {

	}

public:
	virtual bool init() {
		return true;
	}

	virtual void on_timer(uint8 uint16) {

	}
};

#endif // !_ENTITY_H_

