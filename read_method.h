#ifndef MXM_READTYPE_H
#define MXM_READTYPE_H

#include "type.h"
#define MAX_SIZE 4

void saveInfo_byOld(unsigned int oldnum, unsigned int newnum, unsigned int startloc, m_size_t byte);

void saveInfo(unsigned int newnum, unsigned int startloc, m_size_t byte);

unsigned int readInfo(unsigned int startloc, m_size_t byte);


#endif