#include "pageselector.h"
#include "bottom.h"
#include <stdio.h>

extern void free_allocate(m_size_t* free_mem_page, m_size_t free_mem_length, m_size_t* free_mem_position, m_size_t* free_disk_page, m_size_t free_disk_length, m_size_t* free_disk_position)
{
	if (free_mem_length != 0)
	{
		m_size_t tail = readInfo(MEMIDLEHEAP_TAIL_POSITION, MEMIDLEHEAP_TAIL_LENGTH);
		m_size_t save_tail = tail;
		m_size_t head = readInfo(MEMIDLEHEAP_HEAD_POSITION, MEMIDLEHEAP_HEAD_LENGTH);
		m_size_t save_head = head;
		for (m_size_t i = 0; i < free_mem_length; i++)
		{
			m_size_t page_ptr = free_mem_position[i];
			data_unit temp = mem_read(PAGE_BITMAP_START + page_ptr);
			p_address p_tail = MEMIDLEHEAP_STARTPAGE + tail*MEMIDLEHEAP_ENTRY_LENGTH;
			if (temp == 255)
			{
				if (p_tail == MEMIDLEHEAP_ENDPAGE)
				{
					head--;
					if (head < 0)
					{
						printf("head error");
					}
					p_address p_head = MEMIDLEHEAP_STARTPAGE + head*MEMIDLEHEAP_ENTRY_LENGTH;
					saveInfo(page_ptr, p_head, MEMIDLEHEAP_ENTRY_LENGTH);
				}
				else
				{
					saveInfo(page_ptr, p_tail, MEMIDLEHEAP_ENTRY_LENGTH);
					tail++;
				}
			}
			if (temp >= free_mem_page[page_ptr])
			{
				temp = temp - free_mem_page[page_ptr];
				mem_write(temp, (PAGE_BITMAP_START + page_ptr));
			}
		}
		if (tail != save_tail)
		{
			saveInfo_byOld(save_tail, tail, MEMIDLEHEAP_TAIL_POSITION, MEMIDLEHEAP_TAIL_LENGTH);
		}
		if (head != save_head)
		{
			saveInfo_byOld(save_head, head, MEMIDLEHEAP_HEAD_POSITION, MEMIDLEHEAP_HEAD_LENGTH);
		}
	}

	if (free_disk_length != 0)
	{
		m_size_t tail = readInfo(DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
		m_size_t save_tail = tail;
		m_size_t head = readInfo(DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
		m_size_t save_head = head;
		for (m_size_t i = 0; i < free_disk_length; i++)
		{
			m_size_t page_ptr = free_disk_position[i];
			data_unit temp = mem_read(PAGE_BITMAP_DISK_START + page_ptr);
			p_address p_tail = DISKIDLEHEAP_STARTPAGE + tail*DISKIDLEHEAP_ENTRY_LENGTH;
			if (temp == 255)
			{
				if (p_tail == DISKIDLEHEAP_ENDPAGE)
				{
					head--;
					if (head < 0)
					{
						printf("head error");
					}
					p_address p_head = DISKIDLEHEAP_STARTPAGE + head*DISKIDLEHEAP_ENTRY_LENGTH;
					saveInfo(page_ptr, p_head, DISKIDLEHEAP_ENTRY_LENGTH);
				}
				else
				{
					saveInfo(page_ptr, p_tail, DISKIDLEHEAP_ENTRY_LENGTH);
					tail++;
				}
			}
			if (temp >= free_disk_page[page_ptr])
			{
				temp = temp - free_disk_page[page_ptr];
				mem_write(temp, (PAGE_BITMAP_DISK_START + page_ptr));
			}
		}
		if (tail != save_tail)
		{
			saveInfo_byOld(save_tail, tail, DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
		}
		if (head != save_head)
		{
			saveInfo_byOld(save_head, head, DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
		}
	}
}
void free_page(m_size_t page_num)
{
	m_size_t offset = page_num / 8;
	data_unit unit = mem_read(PAGE_BITMAP_DISK_START + offset);
	int bit = page_num % 8;
	if (unit == 255)
	{
		m_size_t tail = readInfo(DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
		p_address p_tail = DISKIDLEHEAP_STARTPAGE + tail*DISKIDLEHEAP_ENTRY_LENGTH;
		if (p_tail == DISKIDLEHEAP_ENDPAGE)
		{
			m_size_t head = readInfo(DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
			head--;
			if (head < 0)
			{
				printf("heap_error!\n");
			}
			p_address p_head = DISKIDLEHEAP_STARTPAGE + head*DISKIDLEHEAP_ENTRY_LENGTH;
			saveInfo(offset, p_head, DISKIDLEHEAP_ENTRY_LENGTH);
			saveInfo_byOld(head + 1, head, DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
		}
		else
		{
			saveInfo(offset, p_tail, DISKIDLEHEAP_ENTRY_LENGTH);
			saveInfo_byOld(tail, tail + 1, DISKIDLEHEAP_TAIL_POSITION, DISKIDLEHEAP_TAIL_LENGTH);
		}
	}
	unit = unit - (1 << bit);
	mem_write(unit, PAGE_BITMAP_DISK_START + offset);
}
m_size_t select_diskpage()
{
	m_size_t bitptr = readInfo(DISK_BITMAP_PTR_POSITION, DISK_BITMAP_PTR_LENGTH);
	m_size_t pagenum = -1;
	if (bitptr < DISK_BITMAP_MAX)
	{
		data_unit temp = mem_read(PAGE_BITMAP_DISK_START + bitptr);
		for (int i = 0; i < 8; i++)
		{
			int j = temp&(1 << i);
			if (j == 0)
			{
				temp = temp + (1 << i);
				pagenum = bitptr * 8 + i;
				mem_write(temp, PAGE_BITMAP_DISK_START + bitptr);
				if (i == 7)
				{
					bitptr++;
					saveInfo_byOld(bitptr - 1, bitptr, DISK_BITMAP_PTR_POSITION, DISK_BITMAP_PTR_LENGTH);
				}
				break;
			}
		}
	}
	else
	{
		m_size_t mem_head = readInfo(DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
		p_address p_head = mem_head*DISKIDLEHEAP_ENTRY_LENGTH + DISKIDLEHEAP_STARTPAGE;
		m_size_t byte_offset = readInfo(p_head, DISKIDLEHEAP_ENTRY_LENGTH);
		data_unit temp = mem_read(PAGE_BITMAP_DISK_START + byte_offset);
		for (int i = 0; i < 8; i++)
		{
			int j = temp&(1 << i);
			if (j == 0)
			{
				temp = temp + (1 << i);
				pagenum = byte_offset * 8 + i;
				mem_write(temp, PAGE_BITMAP_DISK_START + byte_offset);
				if (temp == 255)
				{
					mem_head = mem_head++;
					saveInfo_byOld(mem_head - 1, mem_head, DISKIDLEHEAP_HEAD_POSITION, DISKIDLEHEAP_HEAD_LENGTH);
				}
				break;
			}
		}
	}
	if (pagenum == -1)
	{
		printf("disk_pick_error\n");
	}
	return pagenum;
}
m_size_t select_mempage()
{	
	m_size_t bitptr = readInfo(MEM_BITMAP_PTR_POSITION, MEM_BITMAP_PTR_LENGTH);
	m_size_t pagenum = -1;
	if (bitptr < MEM_BITMAP_MAX)
	{
		data_unit temp = mem_read(PAGE_BITMAP_START + bitptr);
		for (int i = 0; i < 8; i++)
		{
			int j = temp&(1 << i);
			if (j == 0)
			{
				temp = temp + (1 << i);
				pagenum = bitptr * 8 + i;
				mem_write(temp, PAGE_BITMAP_START + bitptr);
				if (i == 7)
				{
					bitptr++;
					saveInfo_byOld(bitptr - 1, bitptr, MEM_BITMAP_PTR_POSITION, MEM_BITMAP_PTR_LENGTH);
				}
				break;
			}
		}
	}
	else
	{
		m_size_t mem_head = readInfo(MEMIDLEHEAP_HEAD_POSITION, MEMIDLEHEAP_HEAD_LENGTH);
		p_address p_head = mem_head*MEMIDLEHEAP_ENTRY_LENGTH + MEMIDLEHEAP_STARTPAGE;
		m_size_t byte_offset = readInfo(p_head, MEMIDLEHEAP_ENTRY_LENGTH);
		data_unit temp = mem_read(PAGE_BITMAP_START + byte_offset);
		for (int i = 0; i < 8; i++)
		{
			int j = temp&(1 << i);
			if (j == 0)
			{
				temp = temp + (1 << i);
				pagenum = byte_offset * 8 + i;
				mem_write(temp, PAGE_BITMAP_START + byte_offset);
				if (temp==255)
				{
					mem_head = mem_head++;
					saveInfo_byOld(mem_head - 1, mem_head, MEMIDLEHEAP_HEAD_POSITION, MEMIDLEHEAP_HEAD_LENGTH);
				}
				break;
			}
		}
	}
	if (pagenum == -1)
	{
		printf("mem_pick_error\n");
	}
	return pagenum;
}