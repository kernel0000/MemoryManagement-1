#ifndef MXM_APPROXIMATELRU_H
#define MXM_APPROXIMATELRU_H

#include "type.h"
#include "const.h"
#include "read_method.h"
#include "address_tranlator.h"
//table_entry只需要高24位
extern p_address load_page(p_address p_table, m_size_t table_entry);
extern void use_again(m_size_t page_num);
#endif