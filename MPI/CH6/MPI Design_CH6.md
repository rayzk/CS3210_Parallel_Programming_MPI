###5.2 MPI Programming Convention

* all var, const have `MPI_` prefix
* MPI 有30个有效符
* 在 `mpif.h` 文件中包含程序中的变量名和类型说明

---

##CH6 六个接口构成的MPI子集

* MPI-1 中有128个接口
* MPI-2 中有287个接口


###6.1 子集介绍
---
**参数说明**

* IN 输入：MPI除使用参数外，不对参数做任何修改
* OUT 输出： MPI返回给调用部分的结果参数，参数初始值对MPI没有任何影响
* INOUT	： 调用部分将参数传递给MPI，MPI引用并修改，将结果返回外部调用
* 

**MPI 初始化**

	MPI_INIT()
	int MPI_Init(int *argc, char ***argv)
	
**MPI 结束**

	MPI_FINALIZE()
	int MPI_Finalize(void)
	
**当前进程标识**

	MPI_COMM_RANK (comm, rank)
	IN comm --- 进程所在通讯域
	OUT rank --- 调用进程在comm中的标识号
	int MPI_Comm_rank (MPI_Comm comm, int *rank)
	
**通讯域包含的进程数**

	MPI_COMM_SIZE (comm, size)
	IN comm 通信域
	OUT size 通信域comm包含的进程数
	int MPI_Comm_size (MPI_Comm comm, int *size)
	
**消息发送**
		
	MPI_SEND (buf, count, datatype, dest, tag, comm)
	所有的para都为IN型
	int MPI_Send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
	
* MPI_SEND 将send_buff中 count * len(datatype) 的数据发送到目标进程

**消息接收**

	MPI_RECV(buf, count, datatype, source, tag, comm, status)
	OUT buf 缓冲区的起始地址
	OUT status 返回状态
	
* 接受的缓冲区由连续的数组空间组成
* 如果短于buff的消息到达，只有对应消息的地址被更改
* status返回状态：
	* MPI_SOURCE
    * MPI_TAG
    * MPI_ERROR
===
###6.2 MPI 预定义数据类型

* MPI_BYTE ：one byte

---

###6.3 MPI数据类型匹配和数据转换
---
**MPI消息传递阶段**

* 装配，传递，拆卸需要匹配类型
	* 宿主语言的类型与MPI指定类型相同
	* 发送与接受的类型匹配
* 有类型的通信，应用数据类型
* 无类型数据的通信： MPI_BYTE 用于不加修改的传送二进制
* 打包数据的通信： MPI_PACKED 用于数据的打包/解包

**数据的转换**

* 数据类型的转换
	* e.g. 实型到整型
	* MPI严格要求类型匹配
* 数据表示的转换
	* e.g. 对于异构系统的浮点转换

---

###6.4 MPI消息

---

**MPI消息的组成**

* 信封：dest, tag, comm
* 数据：but, count, datatype

**MPI通信域**

* 通信域 comm_world
	* 进程组
		* 编号0 - N-1的id
	* 通信上下文
		* 提供独立的通信区域
* 通信域提供了对通信模式的封装


##*The end*

	


