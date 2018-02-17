#ifndef MXM_SELECOTR_H
#define MXM_SELECOTR_H

#include "type.h"
#include "const.h"
#include "read_method.h"

extern m_size_t select_mempage();
extern m_size_t select_diskpage();
extern void free_page(m_size_t page_num);
extern void free_allocate(m_size_t* free_mem_page, m_size_t free_mem_length, m_size_t* free_mem_position, m_size_t* free_disk_page, m_size_t free_disk_length, m_size_t* free_disk_position);

#endif