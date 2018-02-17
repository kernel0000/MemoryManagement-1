#ifndef MXM_PAGETABLE_H
#define MXM_PAGETABLE_H

#include "type.h"
#include "const.h"
#include "address_tranlator.h"
#include "read_method.h"



int free_table(v_address address, m_pid_t pid);
void inittable();
int read_byPageTable(data_unit *data, v_address address, m_pid_t pid);
int write_byPageTable(data_unit data, v_address address, m_pid_t pid);
int allocate_table(v_address* address, m_size_t size, m_pid_t pid);
void detail();
#endif

