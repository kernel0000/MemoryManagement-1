#include <stdio.h>
#include "pagetable.h"
#include "bottom.h"
#include "approximateLRU.h"
#include "pageselector.h"
int transAddress(v_address vaddress, p_address* paddress, m_pid_t pid,int isRead);
int process_segement(v_address start, m_size_t size, p_address dir_start, m_size_t sgement_num, m_size_t segement_index);
m_size_t ceil(m_size_t num, m_size_t size);
m_size_t ceil(m_size_t num, m_size_t size)
{
	if (num <= 0)
	{
		return 0;
	}
	m_size_t result = num / size;
	if (num%size != 0)
	{
		result = result + 1;
	}
	return result;
}
int canRelease(m_size_t range_start, m_size_t range_end, m_size_t* seg_start, m_size_t* seg_end, m_size_t seg_length, m_size_t index);
void free_tablepage(m_size_t* free_table, m_size_t length);
void detail()
{
	printf("------------------------------------------\n");
	printf("total_page:%d\n", readInfo(TOTALPAGE_POSITION, TOTALPAGE_LENGTH));
	printf("record_mem_page:%d\n", readInfo(MEMPAGE_POSITION, MEMPAGE_LENGTH));
	m_size_t count = 0;
	for (m_size_t i = BITMAP_START; i < MEM_BITMAP_MAX; i++)
	{
		data_unit temp = mem_read(PAGE_BITMAP_START + i);
		for (int k = 0; k < 8; k++)
		{
			int j = temp&(1 << k);
			if (j != 0)
			{
				count++;
			}
		}
	}
	printf("realize_mem_page:%d\n", count);
	count = 0;
	for (m_size_t i = 0; i < DISK_BITMAP_MAX; i++)
	{
		data_unit temp = mem_read(PAGE_BITMAP_DISK_START + i);
		for (int k = 0; k < 8; k++)
		{
			int j = temp&(1 << k);
			if (j != 0)
			{
				count++;
			}
		}
	}
	printf("realize_disk_page:%d\n", count);
	
	printf("record_table_page:%d\n", readInfo(LAST_TABLE_POSITION, LAST_TABLE_LENGTH));
	count = 0;
	printf("~~~~~~~~~~~~~~~~~\n");
	for (m_size_t i = 0; i < (MAX_TABLE_PAGE / 8); i++)
	{
		data_unit temp = mem_read(TABLE_BITMAP_START + i);
		for (int k = 0; k < 8; k++)
		{
			int j = temp&(1 << k);
			if (j != 0)
			{
				count++;
				//printf("table_num:%d\n", (i * 8 + k));
			}
		}
	}
	printf("~~~~~~~~~~~~~~~~~\n");
	printf("realize_table_page:%d\n", count);

	m_size_t segement_length = readInfo(PROCESS_INDEX_START + 1*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);
	p_address p_dir_start = getDirEntry(1, 0);
	for (m_size_t i = 0; i < segement_length; i++)
	{
		p_address segement_start = p_dir_start + SEGEMENT_START_OFFSET + i*SEGEMENT_ENTRY_LENGTH;
		m_size_t start = readInfo(segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
		m_size_t end = readInfo(segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
		printf("segement %d:\nstart:%d\nend:%d\n", i, start, end);
	}

	m_size_t mem_heap_head = readInfo(MEMIDLEHEAP_HEAD_POSITION, MEMIDLEHEAP_HEAD_LENGTH);
	m_size_t mem_heap_tail = readInfo(MEMIDLEHEAP_TAIL_POSITION, MEMIDLEHEAP_TAIL_LENGTH);
	printf("MEM heap:\n----head:%d\n----tail:%d\n", mem_heap_head, mem_heap_tail);
	m_size_t disk_heap_head = readInfo(DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
	m_size_t disk_heap_tail = readInfo(DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
	printf("DISK heap:\n----head:%d\n----tail:%d\n", disk_heap_head, disk_heap_tail);
	printf("------------------------------------------\n");
}
void inittable()
{
	saveInfo(0, TOTALPAGE_POSITION, TOTALPAGE_LENGTH);
	saveInfo(0, MEMPAGE_POSITION, MEMPAGE_LENGTH);
	saveInfo(BITMAP_START, MEM_BITMAP_PTR_POSITION, MEM_BITMAP_PTR_LENGTH);
	saveInfo(0, DISK_BITMAP_PTR_POSITION, DISK_BITMAP_PTR_LENGTH);
	saveInfo(0, TABLE_BITMAP_PTR_POSITION, TABLE_BITMAP_PTR_LENGTH);
	saveInfo(0, MEMIDLEHEAP_HEAD_POSITION, MEMIDLEHEAP_HEAD_LENGTH);
	saveInfo(0, MEMIDLEHEAP_TAIL_POSITION, MEMIDLEHEAP_TAIL_LENGTH);
	saveInfo(0, DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
	saveInfo(0, DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
	saveInfo(0, SYS_TIMECLOCK_POSITION, SYS_TIMECLOCK_LENGTH);
	saveInfo(0, LAST_TABLE_POSITION, LAST_TABLE_LENGTH);
	p_address start = PROCESS_INDEX_START;
	for (int i = 0; i < 1000; i++)
	{
		saveInfo(0, start + i*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);
	}
	m_size_t begin = PAGE_BITMAP_START;
	m_size_t end = MIN_PAGE*PAGE_SIZE;
	for (m_size_t i = begin; i < end; i++)
	{
		mem_write(0, i);
	}
}
int free_table(v_address address, m_pid_t pid)
{
	m_size_t segement_length = readInfo(PROCESS_INDEX_START + pid*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);
	if (segement_length == 0)
	{
		return -1;
	}
	p_address p_dir_start = getDirEntry(pid, 0);
	m_size_t start = 0;
	m_size_t end = 0;
	int find = 0;
	m_size_t index = 0;
	m_size_t seg_start[MAX_SEGEMENT];
	m_size_t seg_end[MAX_SEGEMENT];
	for (m_size_t i = 0; i < segement_length; i++)
	{
		p_address segement_start = p_dir_start + SEGEMENT_START_OFFSET + i*SEGEMENT_ENTRY_LENGTH;
		seg_start[i] = readInfo(segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
		seg_end[i] = readInfo(segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
		if (seg_start[i] == address)
		{
			find = 1;
			index = i;
			break;
		}
	}
	if (find == 0)
	{
		return -1;
	}
	start = seg_start[index];
	end = seg_end[index];


	for (m_size_t i = index + 1; i < segement_length; i++)
	{
		p_address segement_start = p_dir_start + SEGEMENT_START_OFFSET + i*SEGEMENT_ENTRY_LENGTH;
		p_address last_segement = p_dir_start + SEGEMENT_START_OFFSET + (i - 1)*SEGEMENT_ENTRY_LENGTH;
		seg_start[i] = readInfo(segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
		seg_end[i] = readInfo(segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
		saveInfo(seg_start[i], last_segement + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
		saveInfo(seg_end[i], last_segement + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
	}

	saveInfo_byOld(segement_length, segement_length - 1, PROCESS_INDEX_START + pid*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);

	m_size_t table_start = start / PAGE_SIZE;
	m_size_t table_end = ceil(end, PAGE_SIZE);
	m_size_t dir_start = start / (PAGE_SIZE*MAX_ENTRY_INPAGE);
	m_size_t dir_end = ceil(end, PAGE_SIZE*MAX_ENTRY_INPAGE);
	m_size_t free_dir[DIR_MAX_LENGTH];
	m_size_t free_dir_length = 0;
	
	m_size_t free_mem_page[MEM_BITMAP_MAX] = { 0 };
	m_size_t free_mem_length = 0;
	m_size_t free_mem_status[MEM_BITMAP_MAX] = { 0 };
	m_size_t free_mem_position[MEM_BITMAP_MAX] = { 0 };
	
	m_size_t free_disk_page[DISK_BITMAP_MAX] = { 0 };
	m_size_t free_disk_length = 0;
	m_size_t free_disk_status[DISK_BITMAP_MAX] = { 0 };
	m_size_t free_disk_position[DISK_BITMAP_MAX] = { 0 };
	
	m_size_t release_page = 0;
 
	m_size_t count1 = 0;

	for (m_size_t i = dir_start; i < dir_end; i++)
	{
		p_address p_dir = getDirEntry(pid, i);
		m_size_t entry = readInfo(p_dir, DIR_ENTRY_LENGTH);
		if (i == dir_start || i == dir_end - 1)
		{
			m_size_t range_start = i*PAGE_SIZE*MAX_ENTRY_INPAGE;
			m_size_t range_end = (i + 1)*PAGE_SIZE*MAX_ENTRY_INPAGE;
			int j = entry&(1 << 11);
			if (canRelease(range_start, range_end, seg_start, seg_end, segement_length, index))
			{
				free_dir[free_dir_length] = (entry << 21) >> 21;
				free_dir_length = free_dir_length + 1;
				mem_write(0, p_dir + 1);
			}
			p_address p_table = 0;
			if (j != 0)
			{
				p_table = getTableEntry(((entry << 21) >> 21), 0);
			}
			for (m_size_t cc = 0; cc < MAX_ENTRY_INPAGE; cc++)
			{
				m_size_t temp_start = range_start + cc*PAGE_SIZE;
				m_size_t temp_end = range_start + (cc + 1)*PAGE_SIZE;
				if (temp_start >= end)
				{
					break;
				}
				if (canRelease(temp_start, temp_end, seg_start, seg_end, segement_length, index))
				{
					release_page++;
					if (j != 0)
					{
						p_address p_temp = p_table + cc*TABLE_ENTRY_LENGTH;
						m_size_t temp = mem_read(p_temp + 2);
						int k = temp&(1 << 3);
						if (k != 0)
						{
							temp = (temp << 16) + readInfo(p_temp, 2);
							m_size_t page_num = (temp << 14) >> 14;
							int inMem = temp&(1 << 18);
							if (inMem == 0)
							{
								m_size_t temp_ptr = page_num / 8;
								m_size_t temp_offset = page_num % 8;
								if (free_disk_status[temp_ptr] == 0)
								{
									free_disk_status[temp_ptr] = 1;
									free_disk_position[free_disk_length] = temp_ptr;
									free_disk_length++;
								}
								free_disk_page[temp_ptr] += (1 << temp_offset);
							}
							else
							{
								m_size_t temp_ptr = page_num / 8;
								m_size_t temp_offset = page_num % 8;
								if (free_mem_status[temp_ptr] == 0)
								{
									free_mem_status[temp_ptr] = 1;
									free_mem_position[free_mem_length] = temp_ptr;
									free_mem_length++;
								}
								free_mem_page[temp_ptr] += (1 << temp_offset);
								count1++;
							}
							saveInfo(0, p_temp + 2, TABLE_ENTRY_LENGTH - 3);
						}
					}
				}
			}
		}
		else
		{
			release_page += 512;
			free_dir[free_dir_length] = (entry << 21) >> 21;
			free_dir_length = free_dir_length + 1;
			int j = entry&(1 << 11);
			mem_write(0, p_dir + 1);
			if (j != 0)
			{
				p_address p_table = getTableEntry(free_dir[free_dir_length - 1], 0);
				for (m_size_t cc = 0; cc < MAX_ENTRY_INPAGE; cc++)
				{
					p_address p_temp = p_table + cc*TABLE_ENTRY_LENGTH;
					m_size_t temp = mem_read(p_temp + 2);
					int k = temp&(1 << 3);
					if (k != 0)
					{
						temp = (temp << 16) + readInfo(p_temp, 2);
						m_size_t page_num = (temp << 14) >> 14;
						int inMem = temp&(1 << 18);
						if (inMem == 0)
						{
							m_size_t temp_ptr = page_num / 8;
							m_size_t temp_offset = page_num % 8;
							if (free_disk_status[temp_ptr] == 0)
							{
								free_disk_status[temp_ptr] = 1;
								free_disk_position[free_disk_length] = temp_ptr;
								free_disk_length++;
							}
							free_disk_page[temp_ptr] += (1 << temp_offset);
						}
						else
						{
							m_size_t temp_ptr = page_num / 8;
							m_size_t temp_offset = page_num % 8;
							if (free_mem_status[temp_ptr] == 0)
							{
								free_mem_status[temp_ptr] = 1;
								free_mem_position[free_mem_length] = temp_ptr;
								free_mem_length++;
							}
							free_mem_page[temp_ptr] += (1 << temp_offset);
							count1++;
						}
						saveInfo(0, p_temp + 2, TABLE_ENTRY_LENGTH - 3);
					}
				}
			}
		}
	}

	m_size_t total = readInfo(TOTALPAGE_POSITION, TOTALPAGE_LENGTH);
	saveInfo_byOld(total, total - release_page, TOTALPAGE_POSITION, TOTALPAGE_LENGTH);
	
	m_size_t mem = readInfo(MEMPAGE_POSITION, MEMPAGE_LENGTH);
	saveInfo_byOld(mem, mem - count1, MEMPAGE_POSITION, MEMPAGE_LENGTH);

	free_allocate(free_mem_page, free_mem_length, free_mem_position, free_disk_page, free_disk_length, free_disk_position);
	if (free_dir_length != 0)
	{
		free_tablepage(free_dir, free_dir_length);
	}
//	detail();
	return 0;
}
int read_byPageTable(data_unit *data, v_address address, m_pid_t pid)
{
	p_address paddress;
	int flag;
	flag = transAddress(address, &paddress, pid, READ_TYPE);
	if (flag == 0)
	{
		if (paddress == -1)
		{
			*data = 0;
			return 0;
		}
		*data = mem_read(paddress);
		return 0;
	}
	else
	{
		return -1;
	}
}
int write_byPageTable(data_unit data, v_address address, m_pid_t pid)
{
	p_address paddress;
	int flag;
	flag = transAddress(address, &paddress, pid, WRITE_TYPE);
	if (flag == 0)
	{
		mem_write(data, paddress);
		return 0;
	}
	else
	{
		return -1;
	}
}
void load_table(m_size_t* table_num, m_size_t need_table)
{
	data_unit save_ptr;
	data_unit ptr = readInfo(TABLE_BITMAP_PTR_POSITION, TABLE_BITMAP_PTR_LENGTH);
	save_ptr = ptr;
	int breakflag = 0;
	m_size_t count = 0;
	while (count < need_table)
	{
		data_unit save;
		data_unit temp = mem_read(TABLE_BITMAP_START + ptr);
		save = temp;
		for (int i = 0; i < 8; i++)
		{
			int j = temp&(1 << i);
			if (j == 0)
			{
				temp = temp + (1 << i);
				table_num[count] = ptr * 8 + i;
				count++;
				if (count == need_table)
				{
					mem_write(temp, TABLE_BITMAP_START + ptr);
					if (temp == 255)
					{
						ptr = ptr + 1;
						if ((ptr + TABLE_BITMAP_START) == TABLE_BITMAP_END)
						{
							ptr = 0;
						}
					}
					if (ptr != save_ptr)
					{
						mem_write(ptr, TABLE_BITMAP_PTR_POSITION);
					}
					breakflag = 1;
					break;
				}
			}
		}
		if (breakflag)
		{
			break;
		}
		if (temp != save)
		{
			mem_write(temp, TABLE_BITMAP_START + ptr);
		}
		ptr++;
		if ((ptr + TABLE_BITMAP_START) == TABLE_BITMAP_END)
		{
			ptr = 0;
		}
	}
}
int allocate_table(v_address* address, m_size_t size, m_pid_t pid)
{
	m_size_t segement_num = readInfo(PROCESS_INDEX_START + pid*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);
	if (segement_num >= MAX_SEGEMENT)
	{
		return -1;
	}
	p_address p_dir = getDirEntry(pid, 0);
	v_address log_add = 0;
	m_size_t segement_index = segement_num;
	int log_flag = 1;
	m_size_t start[MAX_SEGEMENT];
	m_size_t end[MAX_SEGEMENT];
	for (m_size_t i = 0; i < segement_num; i++)
	{
		p_address segement_start = p_dir + SEGEMENT_START_OFFSET + i*SEGEMENT_ENTRY_LENGTH;
		start[i] = readInfo(segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
		end[i] = readInfo(segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
		if (log_flag)
		{
			if (start[i] - log_add >= size)
			{
				segement_index = i;
				log_flag = 0;
				break;
			}
			else
			{
				log_add = end[i];
			}
		}
	}
	if (size + log_add >= MAX_LOGIC_ADDRESS)
	{
		return -1;
	}

	m_size_t need_page = ceil(log_add + size - ceil(log_add, PAGE_SIZE)*PAGE_SIZE, PAGE_SIZE);
	m_size_t totalpage = readInfo(TOTALPAGE_POSITION, TOTALPAGE_LENGTH);
	if (need_page + totalpage > MAX_PAGE - MIN_PAGE)
	{
		return -1;
	}
	m_size_t start_dir = log_add / (PAGE_SIZE*MAX_ENTRY_INPAGE);
	m_size_t end_dir = ceil((log_add + size), (PAGE_SIZE*MAX_ENTRY_INPAGE));
	m_size_t need_table = 0;
	p_address p_entry[DIR_MAX_LENGTH];
	data_unit entry[DIR_MAX_LENGTH];
	for (m_size_t i = start_dir; i < end_dir; i++)
	{
		p_address p_dir = getDirEntry(pid, i);
		data_unit temp = 0;
		if (i == start_dir || i == end_dir - 1)
		{
			temp = mem_read(p_dir + 1);
		}
		int j = temp&(1 << 4);
		if (j == 0)
		{
			data_unit status = 0;
			if (i == end_dir - 1)
			{
				if ((log_add + size) % (PAGE_SIZE*MAX_ENTRY_INPAGE) != 0)
				{
					status = 1;
				}
			}
			entry[need_table] = temp + (1 << 4) + (status << 5);
			p_entry[need_table] = p_dir;
			need_table++;
		}
	}
	m_size_t now_table = readInfo(LAST_TABLE_POSITION, LAST_TABLE_LENGTH);
	if ((now_table + need_table) > MAX_TABLE_PAGE)
	{
		return -1;
	}
	m_size_t table_num[DIR_MAX_LENGTH];
	load_table(table_num, need_table);
	
	for (m_size_t i = 0; i < need_table; i++)
	{
		m_size_t temp = entry[i];
		temp = (temp << 8) + table_num[i];
		saveInfo(temp, p_entry[i], DIR_ENTRY_LENGTH);
	}

	process_segement(log_add, size, p_dir, segement_num, segement_index);
	saveInfo(now_table + need_table, LAST_TABLE_POSITION, LAST_TABLE_LENGTH);
	saveInfo(need_page + totalpage, TOTALPAGE_POSITION, TOTALPAGE_LENGTH);
	saveInfo_byOld(segement_num, segement_num + 1, PROCESS_INDEX_START + pid*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);
	*address = log_add;
//	detail();
	return 0;
}
//segmententry:{32}起始地址{32}终止地址 
int process_segement(v_address start, m_size_t size,p_address dir_start,m_size_t sgement_num,m_size_t segement_index)
{

	m_size_t segement_number = sgement_num;
	if (segement_number >= MAX_SEGEMENT)
	{
		return -1;
	}
	v_address start_add = start;
	v_address end_add = start + size;
	v_address temp_start = start;
	v_address temp_end = start + size;
	for (m_size_t i = segement_index; i <= segement_number; i++)
	{
		p_address segement_start = dir_start + SEGEMENT_START_OFFSET + i*SEGEMENT_ENTRY_LENGTH;
		if (i != segement_number)
		{
			temp_end = readInfo(segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
			temp_start= readInfo(segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
			saveInfo(end_add, segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
			saveInfo(start_add, segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
			start_add = temp_start;
			end_add = temp_end;
		}
		else
		{
			saveInfo(end_add, segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
			saveInfo(start_add, segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
		}
	}
	return 0;
}
int inquiry_segement(v_address vaddress,m_pid_t pid)
{
	m_size_t segement_num = readInfo(PROCESS_INDEX_START + pid*PROCESS_INDEX_INTERVAL, PROCESS_INDEX_INTERVAL);
	p_address dir_start = getDirEntry(pid, 0);
	int flag = 0;
	for (m_size_t i = 0; i < segement_num; i++)
	{
		p_address segement_start = dir_start + SEGEMENT_START_OFFSET + i*SEGEMENT_ENTRY_LENGTH;
		m_size_t end = readInfo(segement_start + SEGEMENT_ENDENTRY_OFFSET, SEGEMENT_ENDENTRY_LENGTH);
		if (end > vaddress)
		{
			m_size_t start = readInfo(segement_start + SEGEMENT_STARTENTRY_OFFSET, SEGEMENT_STARTENTRY_LENGTH);
			if (vaddress >= start)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 0;
}
void active_table(m_size_t table_num)
{
	p_address p_table = getTableEntry(table_num, 0);
	for (m_size_t i = 0; i < MAX_ENTRY_INPAGE; i++)
	{
		saveInfo(0, p_table + i*TABLE_ENTRY_LENGTH, 3);
	}
}
int transAddress(v_address vaddress, p_address* paddress,m_pid_t pid,int isRead)
{
	m_size_t dir_index = vaddress >> 20;
	if (dir_index > DIR_MAX_LENGTH)
	{
		return -1;
	}
	m_size_t table_index = (vaddress << 12) >> 23;
	m_size_t offset = (vaddress << 21) >> 21;
	p_address p_dir = getDirEntry(pid, dir_index);
	m_size_t dir_entry = readInfo(p_dir + 1, 1);
	int flag_in = dir_entry&(1 << 4);
	if (flag_in == 0)
	{
		return -1;
	}
	int flag_inquiry = dir_entry&(1 << 5);
	if (flag_inquiry != 0)
	{
		if (inquiry_segement(vaddress,pid)==0)
		{
			return -1;
		}
	}

	int flag_active = dir_entry&(1 << 3);
	if (flag_active == 0)
	{
		if (isRead)
		{
			*paddress = -1;
			return 0;
		}
		dir_entry = (dir_entry << 8) + readInfo(p_dir, 1);
		m_size_t table_num = (dir_entry << 21) >> 21;
		//active_table(table_num);
		dir_entry = dir_entry + (1 << 11);
		saveInfo(dir_entry >> 8, p_dir + 1, 1);
		p_address p_table = getTableEntry(table_num, table_index);
		p_address add=load_page(p_table, 0);
		*paddress = add + offset;
		return 0;
	}
	else
	{
		dir_entry = (dir_entry << 8) + readInfo(p_dir, 1);
		m_size_t table_num = (dir_entry << 21) >> 21;
		p_address p_table = getTableEntry(table_num, table_index);
		m_size_t table_entry = readInfo(p_table, TABLE_ENTRY_LENGTH - 1);
		int init = table_entry&(1 << 19);
		if (init == 0)
		{		
			if (isRead)
			{
				*paddress = -1;
				return 0;
			}
			p_address add = load_page(p_table, 0);
			*paddress = add + offset;
			return 0;
		}
		else
		{
			int inMem = table_entry&(1 << 18);
			if (inMem == 0)
			{				
				m_size_t page_num = (table_entry << 14) >> 14;
				p_address add = load_page(p_table, table_entry);
				free_page(page_num);
				disk_load(add, page_num*PAGE_SIZE, MAX_OFFSET_IN_PAGE + 1);
				*paddress = add + offset;
				return 0;
			}
			else
			{
				m_size_t page_num = (table_entry << 14) >> 14;
				use_again(page_num);
				p_address add = page_num*PAGE_SIZE;
				*paddress = add + offset;
				return 0;
			}
		}
	}
}
int canRelease(m_size_t range_start, m_size_t range_end, m_size_t* seg_start, m_size_t* seg_end, m_size_t seg_length, m_size_t index)
{
	m_size_t start = seg_start[index];
	m_size_t end = seg_end[index];
	if (start <= range_start&&range_end <= end)
	{
		return 1;
	}
	else if (end <= range_start || range_end <= start)
	{
		return 0;
	}
	else
	{
		int flag = 1;
		for (m_size_t i = 0; i < seg_length; i++)
		{
			if (i == index)
			{
				continue;
			}
			else
			{
				if ((seg_end[i] > range_start&&range_start >= seg_start[i])
					||(seg_start[i]>=range_start&&seg_start[i]<range_end))
				{
					flag = 0;
					break;
				}
				if (seg_start[i] >= range_end)
				{
					break;
				}
			}
		}
		return flag;
	}
}
void free_tablepage(m_size_t* free_table, m_size_t length)
{
	m_size_t last_table = readInfo(LAST_TABLE_POSITION, LAST_TABLE_LENGTH);
	saveInfo_byOld(last_table, last_table - length, LAST_TABLE_POSITION, LAST_TABLE_LENGTH);
	int flag = 0;
	data_unit last_ptr = 0;
	data_unit last_value = 0;
	for (m_size_t i = 0; i < length; i++)
	{
		m_size_t table_num = free_table[i];
		data_unit ptr = table_num / 8;
		data_unit offset = table_num % 8;
		if (flag == 0)
		{
			last_value = mem_read(TABLE_BITMAP_START + ptr);
			last_ptr = ptr;
			int j = last_value&(1 << offset);
			if (j != 0)
			{
				last_value = last_value - (1 << offset);
			}
			flag = 1;
		}
		else
		{
			if (last_ptr == ptr)
			{
				int j = last_value&(1 << offset);
				if (j != 0)
				{
					last_value = last_value - (1 << offset);
				}
			}
			else
			{
				mem_write(last_value, TABLE_BITMAP_START + last_ptr);
				last_value = mem_read(TABLE_BITMAP_START + ptr);
				last_ptr = ptr;
				int j = last_value&(1 << offset);
				if (j != 0)
				{
					last_value = last_value - (1 << offset);
				}
			}
		}
	}
	if (flag)
	{
		mem_write(last_value, TABLE_BITMAP_START + last_ptr);
	}
}