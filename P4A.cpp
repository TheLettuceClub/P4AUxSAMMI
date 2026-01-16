#include "P4A.h"

const char* CharData::currActGet() const {
	return &currAction[0];
}

const char* CharData::prevActGet() const {
	return &prevAction[0];
}