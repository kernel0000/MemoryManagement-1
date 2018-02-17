#include "call.h"
#include "bottom.h"
#include <stdio.h>

#define Fail(tips) \
        printf(tips); \
        printf("\n");\
        return -1;

#define Success(tips) \
        printf(tips); \
        printf("\n");\
        return 0;

//同一个进程申请空间时的释放和再次申请
int test_lyx_1()
{
	init();

	v_address va, vb;
	if (allocate(&va, 1024, 1) != 0 || allocate(&vb, 1024, 1) != 0)//进程1申请2次1024的空间，则可读范围va~va+2047
	{
		Fail("test1: fail, allocation_1");
	}

	data_unit d;
	if (free(0,1) != 0)//释放va~va+1023的空间
	{
		Fail("test1: free");
	}

	if (read(&d,va+1024, 1) != 0 || read(&d, va+256, 1) != -1)//读取va+1024检测后面一块空间是否被释放，读取va+256检测释放的空间是否还能再读取
	{
		Fail("test1: fail, illegal access");
	}

	if (allocate(&vb, 1024, 1) == 0);//重新申请1024的空间
	{
		if (vb == va)
		{
			Success("test1: pass");
		}
		//正常情况下由于第一次申请的地址起释放了1024空间，最好的情况下应当返回进程1第一次申请的地址。
		Fail("test1: fail, allocation_2");
	}

}

//对于超过内存空间的内容的存储和读取正确性的验证
int test_lyx_2()
{
	init();
	m_size_t size = 1024 * 1024 * 320;
	m_size_t interval = 1024;//尽量保证每次读调取一个页面，不会有页面遗漏
	v_address va;
	if (allocate(&va, size, 1) != 0)
	{
		Fail("test2: fail, allocation");
	}
	for (m_size_t i = 1024 * 1024 * 300; i >= 1024 * 1024 * 120; i -= interval)//从后向前读，防止一些直接设定好的情况
	{
		m_size_t temp = i;
		if (write(temp, va + i, 1) != 0 || write(temp >> 8, va + i + 1, 1) != 0
			|| write(temp >> 16, va + i + 2, 1) != 0 || write(temp >> 24, va + i + 3, 1) != 0)//给每一块填入一个id，读取时根据id判断页面交换是否出错
		{
			Fail("test2: fail, big write");
		}
	}
	for (m_size_t i = 1024 * 1024 * 300; i >= 1024 * 1024 * 120; i -= interval)//读取之前填入id的页面，判断值是否和原先一致
	{
		m_size_t temp = i;
		data_unit a;
		data_unit b;
		data_unit c;
		data_unit d;
		if (read(&a, va + i, 1) != 0 || read(&b, va + i + 1, 1) != 0
			|| read(&c, va + i + 2, 1) != 0 || read(&d, va + i + 3, 1) != 0)
		{
			Fail("test2: fail, big read_1");
		}
		temp = a + (b << 8) + (c << 16) + (d << 24);
		if (temp != i)
		{
			Fail("test2: fail, big read_2");
		}
	}
	Success("test2: pass");
}

//对于内存填满情况下页面替换策略的正确性（如果是fifo就会被替换）
int test_lyx_3()
{
	init();
	m_size_t size = 1024 * 1024 * 320;
	m_size_t interval = 1024;
	v_address va;
	if (allocate(&va, size, 1) != 0)
	{
		Fail("test3: fail, allocation");
	}
	//填充满内存
	for (m_size_t i = 1024 * 1024 * 300; i >= 1024 * 1024 * 180; i -= interval)
	{
		m_size_t temp = i;
		if (write(temp, va + i, 1) != 0 || write(temp >> 8, va + i + 1, 1) != 0
			|| write(temp >> 16, va + i + 2, 1) != 0 || write(temp >> 24, va + i + 3, 1) != 0)
		{
			Fail("test3: fail, big write");
		}
	}

	//重新调用280~300区间的页面，让这个区间内的页面重新调入序列(如果是普通的fifo此时便不会改变序列)
	for (m_size_t i = 1024 * 1024 * 300; i >= 1024 * 1024 * 280; i -= interval)
	{
		m_size_t temp = i;
		if (write(temp, va + i, 1) != 0 || write(temp >> 8, va + i + 1, 1) != 0
			|| write(temp >> 16, va + i + 2, 1) != 0 || write(temp >> 24, va + i + 3, 1) != 0)
		{
			Fail("test3: fail, big write");
		}
	}
	//替换页面（替换掉队列末尾的页面）
	for (m_size_t i = 0; i <= 1024 * 1024 * 20; i += 2 * interval)
	{
		m_size_t temp = i;
		if (write(temp, va + i, 1) != 0 || write(temp >> 8, va + i + 1, 1) != 0
			|| write(temp >> 16, va + i + 2, 1) != 0 || write(temp >> 24, va + i + 3, 1) != 0)
		{
			Fail("test3: fail, big write");
		}
	}
	count_t ma, mb, da, db;
	evaluate(&ma, &mb, &da, &db);
	//重新调用280~300区间的页面（如果在上面替换时被替换掉，便会从交换区交换回来，那么这种页面置换策略不合理）
	for (m_size_t i = 1024 * 1024 * 300; i >= 1024 * 1024 * 280; i -= interval)
	{
		m_size_t temp = i;
		if (write(temp, va + i, 1) != 0 || write(temp >> 8, va + i + 1, 1) != 0
			|| write(temp >> 16, va + i + 2, 1) != 0 || write(temp >> 24, va + i + 3, 1) != 0)
		{
			Fail("test3: fail, big write");
		}
	}
	count_t ma2, mb2, da2, db2;
	evaluate(&ma2, &mb2, &da2, &db2);
	if (da2 - da > 400000 || db2 - db > 400000)//期望交换次数应该为0
	{
		Fail("test3: disk access");
	}
	Success("test3: pass");
}


//释放空间
int test_lyx_4()
{
	init();
	m_size_t size = 1024 * 1024 * 1;
	m_size_t interval = 1024;
	v_address va[501];
	data_unit t;
	for (int i = 0; i < 500; i++)//申请500m的空间
	{
		if (allocate(&va[i], size, i) != 0)
		{
			Fail("test4: fail, allocation");
		}
	}
	for (int i = 0; i < 500; i++)//判断是否申请属实，防止一下偷懒的情况
	{
		data_unit temp = i;
		if (write(temp, va[i], i) != 0)
		{
			Fail("test4: fail, write");
		}
		if (read(&t, va[i], i) != 0)
		{
			Fail("test4: fail, read");
		}
		if (t != temp)
		{
			Fail("test4: fail, read");
		}
	}
	for (int i = 0; i < 500; i += 2)//释放空间，隔一个释放一个，则有250m的碎片空间和剩余140m的连续空间
	{
		if (free(va[i], i) != 0)
		{
			Fail("test4: free");
		}
	}
	if (allocate(&va[500], size*350, 500) != 0)//申请350m的空间
	{
		Fail("test4: fail, allocation");
	}
	if (write(255, va[500], 500) != 0)//判断是否属实
	{
		Fail("test4: fail, write");
	}
	if (read(&t, va[500], 500) != 0)
	{
		Fail("test4: fail,big read_1");
	}
	if (t != 255)
	{
		Fail("test4: fail,big read_2");
	}
	Success("test4: pass");
}

int test_lyx_5()
{
	init();

	m_size_t size = 1024 * 1024 * 320;

	v_address va;
	data_unit t;

	if (allocate(&va, size, 500) != 0)//申请超过内存大小的空间
	{
		Fail("test5: fail, allocation");
	}
	count_t ma, mb, da, db;
	evaluate(&ma, &mb, &da, &db);
	for (int i = 0; i < 1024 * 1024 * 300; i += 512)//隔512字节读取一次，理论上未初始化的情况下期望读取次数非常少，并且不用做页面替换，保证每一页都有读取到
	{
		if (read(&t, va + i, 500) != 0)
		{
			Fail("test5: fail, read");
		}
	}
	count_t ma2, mb2, da2, db2;
	evaluate(&ma2, &mb2, &da2, &db2);

	int times = 2;  //允许非初始化情况下每次访问读取的次数

	m_size_t limit = 1024 * 300 * 3 * times;

	if ((ma2 - ma) > limit)
	{
		Fail("test5: fail, mem access");
	}
	Success("test5: pass");
}