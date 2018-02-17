#ifndef MXM_CONST_H
#define MXM_CONST_H
//v_address:{2} {10}ҳĿ¼�� {9} ҳ��ҳ�� {11}ҳ��ƫ����
//pageentry: {2}{10}{1}�Ƿ��ʼ��{1}�Ƿ����ڴ� {18}ҳ��                             ////{11}ҳ�����ƫ����
//direntry:  {1}{2}״̬��00ֱ�Ӷ�ȡ��01���ѯ����Ϣ){1}����״̬ {1}�Ƿ񼤻� {11}ҳ��ҳ 
//�ڴ���ж�{16}��λʾͼ�е�ƫ����  8ҳ        Ӳ�̿��ж� {16}��λʾͼ�е�ƫ����   32ҳ

//{3}ռ����ҳ�� {2} ռ�����ڴ�ҳ��
//{2}�ڴ�ռ�λʾͼָ�� {2}Ӳ�̿ռ�λʾͼָ�� {1}ҳ��ҳλʾͼָ�� 
// {2}�ڴ���ж���ָ�� {2}�ڴ���ж�βָ�� {2}Ӳ�̿��ж���ָ��{2}Ӳ�̿��ж�βָ��{4}��ȡʱ�� {2}��ǰҳ��ҳ

//ҳĿ¼��1280b��

//352�������
//segmententry:{32}��ʼ��ַ{32}��ֹ��ַ 

//{{2}����0ҳĿ¼���� {2}����1ҳĿ¼���ȡ�������������{2}����999ҳĿ¼���� 2000} 2022 1��ҳ
//�ڴ���Ӳ�̿ռ��λʾͼ 20��ҳ
//40ҳ�ڴ���Ӳ�̿��ж�
//�ڴ�ҳ��Ӧʱ�� {32}ռ�иÿ�ռ����Ŀ���ڵ�ַ {32}��ȡʱ�� 256��ҳ
//ҳĿ¼��С4000kb��0~2ҳΪ����0ҳĿ¼��2~4ҳΪ����1ҳĿ¼������������������2000��ҳ
//��ҳ��ҳ 4m
///��ռ��8m

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

//ҳ����С
#define MEM_PAGE_SIZE 65536//1024*64
#define DISK_PAGE_SIZE 262144//1024*256
#define MAX_PAGE 327680//1024*320
#define MIN_PAGE 4096
#define PAGE_SIZE 2048//ÿҳ��С
#define MAX_OFFSET_IN_PAGE 2047

//��һҳ������
#define TOTALPAGE_POSITION 0//ռ����ҳ��
#define TOTALPAGE_LENGTH 3

#define MEMPAGE_POSITION 3//ռ���ڴ�ҳ��
#define MEMPAGE_LENGTH 2

#define MEM_BITMAP_PTR_POSITION 5//�ڴ�λʾͼָ��
#define MEM_BITMAP_PTR_LENGTH 2

#define DISK_BITMAP_PTR_POSITION 7//Ӳ��λʾͼָ��
#define DISK_BITMAP_PTR_LENGTH 2

#define TABLE_BITMAP_PTR_POSITION 9//ҳ��ҳλʾͼָ��
#define TABLE_BITMAP_PTR_LENGTH 1

#define MEMIDLEHEAP_HEAD_POSITION 10//�ڴ����ö�ͷָ��
#define MEMIDLEHEAP_HEAD_LENGTH 2

#define MEMIDLEHEAP_TAIL_POSITION 12//�ڴ����ö�βָ��
#define MEMIDLEHEAP_TAIL_LENGTH 2

#define DISKIDLEHEAP_HEAD_POSITION 14//Ӳ�����ö�ͷָ��
#define DISKIDLEHEAP_HEAD_LENGTH 2

#define DISKIDLEHEAP_TAIL_POSITION 16//Ӳ�����ö�βָ��
#define DISKIDLEHEAP_TAIL_LENGTH 2

#define SYS_TIMECLOCK_POSITION 18//ϵͳʱ��
#define SYS_TIMECLOCK_LENGTH 4

#define LAST_TABLE_POSITION 22
#define LAST_TABLE_LENGTH 2

//����Ŀ¼����
#define PROCESS_INDEX_START 24
#define PROCESS_INDEX_INTERVAL 2

//λʾͼ
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

//ʱ�̱�
#define TIME_TABLE_START 126976//62*2048
#define TIME_TABLE_END 651264//318*2048
#define TIME_ENTRY_LENGTH 4
#define TIME_ENTRYINFO_LENGTH 4

//��������Ŀ¼��
#define DIR_ENTRY_LENGTH 2
#define PAGE_DIR_START 651264//318*2048
#define PAGE_DIR_LENGTH 4096
#define PAGE_DIR_END 4747264//2318*2048
#define DIR_MAX_LENGTH 640

#define PAGE_BIT 19//ҳ��λ��

#define TABLE_ENTRY_LENGTH 4

//��ҳ��ҳ
#define PAGE_TABLE_START 4747264//2320*2048
#define PAGE_TABLE_LENGTH 2048
#define PAGE_TABLE_END 8388608//4096*2048

#endif