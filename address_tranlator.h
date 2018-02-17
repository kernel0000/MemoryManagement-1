#ifndef MXM_ADDRESSTRANLATOR_H
#define MXM_ADDRESSTRANLATOR_H

#include "type.h"
#include "const.h"
extern p_address getDirEntry(m_pid_t pid, m_size_t dir_index);
extern p_address getTableEntry(m_size_t table_num, m_size_t table_index);
extern p_address getPageAddress(m_size_t pagenum);
extern p_address getTimeEntry(m_size_t pagenum);

#endif