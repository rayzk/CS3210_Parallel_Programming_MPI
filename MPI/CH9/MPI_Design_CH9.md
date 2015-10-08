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





