#include "approximateLRU.h"
#include "bottom.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include "pageselector.h"
m_size_t eliminate_page(m_size_t sys_clock);
void plus_times(p_address p_table, m_size_t table_entry, m_size_t pagenum);
void record_clock(p_address p_table, m_size_t pagenum, m_size_t sys_clock);
void getRandList(m_size_t* list);
m_size_t getRand();
m_size_t force_eliminate();
void use_again(m_size_t page_num)
{
	m_size_t sys_clock = readInfo(SYS_TIMECLOCK_POSITION, SYS_TIMECLOCK_LENGTH);
	p_address p_clock = getTimeEntry(page_num);
	saveInfo(sys_clock, p_clock, TIME_ENTRY_LENGTH);
	if (sys_clock == MAX_SYSCLOCK)
	{
		sys_clock=force_eliminate();
	}
	sys_clock = sys_clock++;
	saveInfo_byOld(sys_clock - 1, sys_clock, SYS_TIMECLOCK_POSITION, SYS_TIMECLOCK_LENGTH);
}

p_address load_page(p_address p_table, m_size_t table_entry)
{
	m_size_t mem_size = readInfo(MEMPAGE_POSITION, MEMPAGE_LENGTH);
	m_size_t sys_clock = readInfo(SYS_TIMECLOCK_POSITION, SYS_TIMECLOCK_LENGTH);
	m_size_t pagenum;
	if (mem_size < MEM_PAGE_SIZE - MIN_PAGE)
	{
		saveInfo_byOld(mem_size, mem_size + 1, MEMPAGE_POSITION, MEMPAGE_LENGTH);
		pagenum = select_mempage();
		plus_times(p_table, table_entry, pagenum);
		record_clock(p_table, pagenum, sys_clock);
	}
	else
	{
		pagenum = eliminate_page(sys_clock);
		plus_times(p_table, table_entry, pagenum);
		record_clock(p_table, pagenum, sys_clock);
	}
	if (sys_clock == MAX_SYSCLOCK)
	{
		sys_clock = force_eliminate();
	}
	sys_clock = sys_clock++;
	saveInfo_byOld(sys_clock - 1, sys_clock, SYS_TIMECLOCK_POSITION, SYS_TIMECLOCK_LENGTH);
	p_address result = getPageAddress(pagenum);
	return result;
}
m_size_t eliminate_page(m_size_t sys_clock)
{
	int count = 0;
	m_size_t min = sys_clock;
	m_size_t minrank = 0;
	m_size_t randomlist[PICK_NUMBER];
	getRandList(randomlist);
	for (int i = 0; i < PICK_NUMBER; i++)
	{
		p_address temp = getTimeEntry(randomlist[i]);
		m_size_t time = readInfo(temp, TIME_ENTRY_LENGTH);
		if (time < min)
		{
			min = time;
			minrank = i;
		}
	}
	m_size_t eliminate_page = randomlist[minrank];
	while ((sys_clock - HOLD_LIMIT) <= min)
	{
		m_size_t page = getRand();
		if (page >= MIN_PAGE)
		{
			p_address temp = getTimeEntry(page);
			m_size_t time = readInfo(temp, TIME_ENTRY_LENGTH);
			if (time < min)
			{
				min = time;
				eliminate_page = page;
			}
		}
	}

	p_address p_eliminate = getTimeEntry(eliminate_page);
	p_address p_info = readInfo(p_eliminate + TIME_ENTRY_LENGTH, TIME_ENTRYINFO_LENGTH);
	m_size_t table_entry = readInfo(p_info, TABLE_ENTRY_LENGTH - 1);
	m_size_t pagenum = select_diskpage();
	m_size_t offset = MAX_OFFSET_IN_PAGE;
	
	/*m_size_t z2 = 0;
	p_address disk = getPageAddress(pagenum);
	for (int i = 0; i < 4; i++)
	{
		z2 = z2 + (disk_read(disk + i) << (i * 8));
	}*/

	disk_save(eliminate_page*PAGE_SIZE, pagenum*PAGE_SIZE, offset + 1);

	m_size_t temp_table = (1 << 19) + pagenum;
	saveInfo_byOld(table_entry, temp_table, p_info, 3);
	//data_unit temp1 = disk_read(pagenum*PAGE_SIZE);
	//data_unit temp = disk_read(pagenum*PAGE_SIZE);
	//m_size_t zzz = (table_entry << 2) >> 13;
	//m_size_t aaa = (temp_table << 8 )+ ((table_entry << 24) >> 24);

	//m_size_t bbb = readInfo(p_info, 4);
	//if (1)
	//{
	//	printf("hahahhaah\n%d,%x,%x,%x\n", temp, p_info, bbb, aaa);
	//	printf("%x:-:%x:-:%x\n", (table_entry<<2)>>13, eliminate_page,pagenum);
	//	m_size_t z1 = 0;
	//	p_address disk = getPageAddress(pagenum);
	//	for (int i = 0; i < 4; i++)
	//	{
	//		z1 = z1 + (disk_read(disk + i) << (i * 8));
	//	}
	//	printf("after:%x,before:%x\n", z1, z2);
	//}
	return eliminate_page;
}
void getRandList(m_size_t* list)
{
	srand((unsigned int)(time(NULL) + rand() + (rand() << 15) + (rand() << 30)));
	m_size_t count = 0;
	int i = 0;
	m_size_t firstnumber = 0;
	int num = 1 << 15;
	int flag = 0;
	while (count < PICK_NUMBER)
	{
		if (i == 0 || i == 16)
		{
			i = 0;
			firstnumber = rand();
		}
		flag = firstnumber&(1 << i);
		if (flag != 0)
		{
			flag = num;
		}
		m_size_t a = rand() + flag;
		i++;
		if (a >= MIN_PAGE)
		{
			list[count] = a;
			count++;
		}
	}
}
m_size_t getRand()
{
	m_size_t result = rand();
	result = (result << 31) >> 16;
	result = result + rand();
	return result;
}
void plus_times(p_address p_table, m_size_t table_entry, m_size_t pagenum)
{
	m_size_t temp_table = ((unsigned)3 << 18) + pagenum;
	//pageentry: {2}{10}{1}是否初始化{1}是否在内存 {18}页号                             ////{11}页内最大偏移量
	//direntry:  {1}{2}状态（00直接读取，01需查询段信息){1}申请状态 {1}是否激活 {11}页表页 
	saveInfo_byOld(table_entry, temp_table, p_table, 3);
	/*m_size_t bbb = readInfo(p_table, 4);
	m_size_t zzz = (aaa << 2) >> 13;
	if (pagenum==4096)
	{
		printf("%x\n", p_table);
		printf("%x------%x\n",aaa,bbb);
	}*/
}
void record_clock(p_address p_table, m_size_t pagenum, m_size_t sys_clock)
{
	p_address p_clock = getTimeEntry(pagenum);
	saveInfo(sys_clock, p_clock, TIME_ENTRY_LENGTH);
	saveInfo(p_table, p_clock + TIME_ENTRY_LENGTH, TIME_ENTRYINFO_LENGTH);
}
m_size_t force_eliminate()
{
	printf("FORCE　ELIMINATE!!!!!\n");
	for (m_size_t i = 0; i < MEM_PAGE_SIZE; i++)
	{
		p_address temp_time = getTimeEntry(i);
		m_size_t time = readInfo(temp_time, TIME_ENTRY_LENGTH);
		m_size_t save = time;
		if (time > BANISH_LIMIT)
		{
			time = time - BANISH_LIMIT;
		}
		else
		{
			time = 0;
		}
		saveInfo_byOld(save, time, temp_time, TIME_ENTRY_LENGTH);
	}
	return MAX_SYSCLOCK - BANISH_LIMIT;
}