#CH7 简单的MPI程序实例

###7.1 MPI实现计时功能

**MPI_WTIME interface**

	double MPI_Wtime(void)
	
* 返回浮点数表示的秒数

**MPI_WTICK interface**

	double MPI_Wtick()
	
###7.2 机器名字和MPI版本号

**动态机器名字**

	MPI_GET_PROCESSOR_NAME(name, resultlen)
	OUT name 
	OUT resultless
	int MPI_Get_processor_name(char *name, int *resultlen)
	
**Get version and subversion**

	MPI_GET_VERSION(version, subversion)
	OUT version 
	OUT subversion
	int MPI_Get_version(int *version, int *subversion)
	
