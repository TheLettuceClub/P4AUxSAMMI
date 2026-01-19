#include "P4A.h"

const char* CharData::currActGet() const {
	return &currAction[0];
}

const char* CharData::incomAtkGet() const
{
	return &incomingAttackName[0];
}

const char* CharData::spriteGet() const
{
	return &sprite[0];
}

const char* CharData::prevActGet() const {
	return &prevAction[0];
}