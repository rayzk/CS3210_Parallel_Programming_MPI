#CH9 不同通信模式MPI并行程序设计

** MPI有四中通信模式 **

* standard mode
* buffered mode
* synchronous mode
* ready mode
* 非标准通信模式是有发送操作，而没有相应的接受操作

Communication Mode | Send          | Receive
------------------ | ------------- | ------------
Standard mode      | MPI_SEND      | MPI_RECV
buffered mode      | MPI_BSEND     | 
synchronous mode   | MPI_SSEND     | 
ready mode         | MPI_RSEND     | 

###9.1 Standard Mode

* 是否对发送数据进行缓存是由MPI自身决定的
* 若MPI缓存数据，发送操作不管接收操作是否执行，都可以进行。发送操作可以正确返回而不要求接受操作收到发送的数据

> 缓存数据会付出代价。若MPI不缓存数据，只有等接收调用被执行，发送数据到达缓冲区，发送操作才算完成。

###9.2 Buffered Mode

	MPI_BSEND (buf, count, datatype, dest, tag, comm)
	// in c :
	int MPI_Bsend (void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
	
* Bsend() 使用用户提供的缓存区
* 不管接受调用是否执行，发送调用都可以执行
* 由用户保证缓存区可用

**申请缓冲区**

	MPI_BUFFER_ATTACH (buffer, size)
		IN buffer 
		IN size (count as byte)
	// in c :
	int MPI_Buffer_attach (void* buf, int size)
	
**回收缓冲区**

	MPI_BUFFER_DETACH (buffer, size)
	
###9.3 Synchronous Mode

* 不依赖于接收进程的接收调用是否已经启动
* 同步发送必须等到相应的接收进程开始后

###9.4 Ready Mode

* 只有当接收进程的接收操作启动时，才可以在发送进程启动发送操作
* 否则，发送操作出错






