#include "call.h"
#include "bottom.h"
#include "pagetable.h"
#include <stdio.h>
void init() 
{
	inittable();
}

int read(data_unit *data, v_address address, m_pid_t pid)
{
	return read_byPageTable(data,address,pid);
}

int write(data_unit data, v_address address, m_pid_t pid)
{
	return write_byPageTable(data, address, pid);
}

int allocate(v_address *address, m_size_t size, m_pid_t pid)
{
	return allocate_table(address, size, pid);
}

int free(v_address address, m_pid_t pid)
{
	return free_table(address, pid);
}