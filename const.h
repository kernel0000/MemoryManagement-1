#ifndef MXM_CONST_H
#define MXM_CONST_H
//v_address:{2} {10}页目录号 {9} 页表页号 {11}页内偏移量
//pageentry: {2}{10}{1}是否初始化{1}是否在内存 {18}页号                             ////{11}页内最大偏移量
//direntry:  {1}{2}状态（00直接读取，01需查询段信息){1}申请状态 {1}是否激活 {11}页表页 
//内存空闲堆{16}在位示图中的偏移量  8页        硬盘空闲堆 {16}在位示图中的偏移量   32页

//{3}占用总页数 {2} 占用总内存页数
//{2}内存空间位示图指针 {2}硬盘空间位示图指针 {1}页表页位示图指针 
// {2}内存空闲堆首指针 {2}内存空闲堆尾指针 {2}硬盘空闲堆首指针{2}硬盘空闲堆尾指针{4}读取时钟 {2}当前页表页

//页目录中1280b起

//352项段描述
//segmententry:{32}起始地址{32}终止地址 

//{{2}进程0页目录长度 {2}进程1页目录长度。。。。。。。{2}进程999页目录长度 2000} 2022 1个页
//内存与硬盘空间的位示图 20个页
//40页内存与硬盘空闲堆
//内存页对应时钟 {32}占有该块空间的条目所在地址 {32}读取时钟 256个页
//页目录大小4000kb，0~2页为进程0页目录，2~4页为进程1页目录。。。。。。。。共2000个页
//总页表页 4m
///总占用8m

#define MAX_LOGIC_ADDRESS 0x28000000
#define SEGEMENT_ENTRY_LENGTH 8
#define SEGEMENT_STARTENTRY_OFFSET 4
#define SEGEMENT_STARTENTRY_LENGTH 4
#define SEGEMENT_ENDENTRY_OFFSET 0
#define SEGEMENT_ENDENTRY_LENGTH 4
#define SEGEMENT_START_OFFSET 1280
#define MAX_SEGEMENT 352
#define MAX_TABLE_PAGE 1776

#define MAX_ENTRY_INPAGE 512
#define CONSTANT_LIMIT 512
#define UNDESIRED_PAGE 524287
#define READ_TYPE 1
#define WRITE_TYPE 0
#define EXTEND 1
#define UNEXTEND 0

//LRU
#define NEED_MIN 2320
#define PICK_NUMBER 8
#define BANISH_LIMIT 0xcfffffff
#define MAX_SYSCLOCK 0xffffffff
#define HOLD_LIMIT 32768
#define RETURN_LIMIT 6

//页数大小
#define MEM_PAGE_SIZE 65536//1024*64
#define DISK_PAGE_SIZE 262144//1024*256
#define MAX_PAGE 327680//1024*320
#define MIN_PAGE 4096
#define PAGE_SIZE 2048//每页大小
#define MAX_OFFSET_IN_PAGE 2047

//第一页的配置
#define TOTALPAGE_POSITION 0//占用总页数
#define TOTALPAGE_LENGTH 3

#define MEMPAGE_POSITION 3//占用内存页数
#define MEMPAGE_LENGTH 2

#define MEM_BITMAP_PTR_POSITION 5//内存位示图指针
#define MEM_BITMAP_PTR_LENGTH 2

#define DISK_BITMAP_PTR_POSITION 7//硬盘位示图指针
#define DISK_BITMAP_PTR_LENGTH 2

#define TABLE_BITMAP_PTR_POSITION 9//页表页位示图指针
#define TABLE_BITMAP_PTR_LENGTH 1

#define MEMIDLEHEAP_HEAD_POSITION 10//内存闲置堆头指针
#define MEMIDLEHEAP_HEAD_LENGTH 2

#define MEMIDLEHEAP_TAIL_POSITION 12//内存闲置堆尾指针
#define MEMIDLEHEAP_TAIL_LENGTH 2

#define DISKIDLEHEAP_HEAD_POSITION 14//硬盘闲置堆头指针
#define DISKIDLEHEAP_HEAD_LENGTH 2

#define DISKIDLEHEAP_TAIL_POSITION 16//硬盘闲置堆尾指针
#define DISKIDLEHEAP_TAIL_LENGTH 2

#define SYS_TIMECLOCK_POSITION 18//系统时针
#define SYS_TIMECLOCK_LENGTH 4

#define LAST_TABLE_POSITION 22
#define LAST_TABLE_LENGTH 2

//进程目录长度
#define PROCESS_INDEX_START 24
#define PROCESS_INDEX_INTERVAL 2

//位示图
#define PAGE_BITMAP_START 2048
#define PAGE_BITMAP_DISK_START 10240//5*2048
#define PAGE_BITMAP_END	43008//21*2048
#define BITMAP_START 512
#define MEM_BITMAP_MAX 8192
#define DISK_BITMAP_MAX 32768
#define MEMIDLEHEAP_STARTPAGE 43008//21*2048
#define MEMIDLEHEAP_ENTRY_LENGTH 2
#define MEMIDLEHEAP_ENDPAGE 59392//29*2048
#define DISKIDLEHEAP_STARTPAGE 59392//29*2048
#define DISKIDLEHEAP_ENTRY_LENGTH 2
#define DISKIDLEHEAP_ENDPAGE 124928//61*2048

#define TABLE_BITMAP_START 124928//61*2048
#define TABLE_BITMAP_END 125150

//时刻表
#define TIME_TABLE_START 126976//62*2048
#define TIME_TABLE_END 651264//318*2048
#define TIME_ENTRY_LENGTH 4
#define TIME_ENTRYINFO_LENGTH 4

//各个进程目录表
#define DIR_ENTRY_LENGTH 2
#define PAGE_DIR_START 651264//318*2048
#define PAGE_DIR_LENGTH 4096
#define PAGE_DIR_END 4747264//2318*2048
#define DIR_MAX_LENGTH 640

#define PAGE_BIT 19//页表位数

#define TABLE_ENTRY_LENGTH 4

//总页表页
#define PAGE_TABLE_START 4747264//2320*2048
#define PAGE_TABLE_LENGTH 2048
#define PAGE_TABLE_END 8388608//4096*2048

#endif