#CH7 简单的MPI程序实例

###7.1 MPI实现计时功能

**MPI_WTIME interface**

	// C code
	double MPI_Wtime(void)
	double MPI_Wtick()
	
* 返回 MPI_WTIME 的精确度，单位是秒

**MPI_WTICK interface**

	double MPI_Wtick()
	
###7.2 机器名字和MPI版本号

**Get the name of machine dynamically**

	MPI_GET_PROCESSOR_NAME(name, resultlen)
	OUT name 
	OUT resultless
	int MPI_Get_processor_name(char *name, int *resultlen)
	
**Get version and subversion**

	MPI_GET_VERSION(version, subversion)
	OUT version 
	OUT subversion
	int MPI_Get_version(int *version, int *subversion)
	
###7.3 是否初始化

	MPI_INITIALIZED(flag)
	OUT flag -- MPI_INIT 是否执行的标志
	
**退出程序**

	MPI_ABOURT (comm, errcode)
		IN comm -- 通信域
		IN errorcode -- 环境错误码
	int MPI_Abort (MPI_COMM_WORLD, int errorcode)
	
###7.7 安全的MPI程序

* 安排好发送和接受的顺序，一定要一个发送后接受，另一个接收后发送，形成环状
* 若两个都先接受，造成锁死
* 若两个都先发送，若system buffer满了的话，也会造成锁死现象