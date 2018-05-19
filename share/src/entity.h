
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
};

#endif // !_ENTITY_H_

