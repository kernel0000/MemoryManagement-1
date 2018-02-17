#include <stdio.h>
#include "address_tranlator.h"
inline p_address getDirEntry(m_pid_t pid, m_size_t dir_index)
{
	p_address result = PAGE_DIR_START + pid*PAGE_DIR_LENGTH + dir_index*DIR_ENTRY_LENGTH;
	return result;
}
inline p_address getTableEntry(m_size_t table_num, m_size_t table_index)
{
	p_address result = PAGE_TABLE_START + table_num*PAGE_TABLE_LENGTH + table_index*TABLE_ENTRY_LENGTH;
	return result;
}
inline p_address getPageAddress(m_size_t pagenum)
{
	p_address result = pagenum*PAGE_SIZE;
	return result;
}
inline  p_address getTimeEntry(m_size_t pagenum)
{
	p_address result = TIME_TABLE_START + pagenum*(TIME_ENTRY_LENGTH + TIME_ENTRYINFO_LENGTH);
	return result;
}