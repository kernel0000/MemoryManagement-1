#include "read_method.h"
#include "bottom.h"
#include <stdio.h>
void saveInfo_byOld(unsigned int oldnum, unsigned int newnum, unsigned int startloc, m_size_t size)
{
	if (size > MAX_SIZE)
	{
		size = 4;
	}
	data_unit old[4], new[4];
	for (int i = 0; i < size; i++) {
		old[i] = oldnum >> (8 * i);
		new[i] = newnum >> (8 * i);
		//printf("±È½Ï%d:%d\n", old[i], new[i]);
		if (old[i] != new[i]) {
			mem_write(new[i], startloc + i);
		}
	}
}

void saveInfo(unsigned int newnum, unsigned int startloc, m_size_t size)
{
	if (size > MAX_SIZE)
	{
		size = 4;
	}
	data_unit  new[4];
	for (int i = 0; i < size; i++) {
		new[i] = newnum >> (8 * i);
		mem_write(new[i], startloc + i);
	}
}

unsigned int readInfo(unsigned int startloc, m_size_t size)
{
	if (size > MAX_SIZE)
	{
		size = 4;
	}
	unsigned int result = 0;
	for (int i = 0; i < size; i++)
	{
		data_unit temp = mem_read(startloc + i);
		unsigned int trans = (unsigned)(temp << (i * 8));
		//printf("%d\n",trans);
		result = result + trans;
	}
	return result;
}