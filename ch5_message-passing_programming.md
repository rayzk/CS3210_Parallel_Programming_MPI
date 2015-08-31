#Message-Passing Programming

### Resource
* *__Learning notes__ of __[Parallel Programming for Multicore and Cluster Systems][Amazon_book_link_ref]__* 

* *Official [__MPI__ documents][mpi_docs_ref]*

* *Free available MPI libraries [__OpenMPI__][openmpi_ref]*






### Overview


* Message-passing programming model:
	* Based on distributed address space & processors with exclusively access local memory
	* Topology of the interconnection network is hidden from this model
	* Support SPMD & MPMD
	* Support point-to-point & global _(broadcast)_ communication operations _(usually through libraries)_
	
* Examples:

	* __MPI__ _(Message-Passing Interface)_ is the most popular portable communication library
	* __PVM__ _(Parallel Virtual Machine)_ 







### Introduction to MPI

#### 1.Point-to-point communication



__Run MPI program in Shell :__

	mpiexec -n 4 programname programarguments
	mpirun -np 4 programname programarguments

---
__Blocking send operation :__
	
	int MPI_Send (void *smsg,
				int count,
				MPI_Datatype datatype,
				int dest,
				int tag,
				MPI_Comm communicator)


__Meaning of patameters :__

* `smgs` send buffer
* `count` number of elements from the send buffer
* `datatype` type of each entry in buffer, all entries have same type [^1]
* `dest` rank of the target process
* `tag` used by receiver to distinguish sender __integer value between 0 and 32767__
* `comm` communicator
* size of the msg is computed be multiplying the `count` with bytes of certain `datatype`


---



__blocking receive operation__

	
	int MPI_Recv(void *rmsg,
					int count,
					MPI_Datatype datatype,
					int source,
					int tag,
					MPI_Comm comm,
					MPI_Status *status)


__Meaning of parameters :__

* `rmsg` receive buffer
* `count` max number of elements
* `datatype` ~
* `source` rank of sending process
* `tag` must have
* `comm` communicator
* `status` data structure contains info of message after completion of receive operation
* `source = MPI_ANY_SOURCE` can receive msg from any arbitrary process, similar to `tag = MPI_ANY_TAG`
* __`status`__ contains :
	* `status.MPI_SOURCE` rank of sending process
	* `status.MPI_TAG` tag of msg received
	* `status.MPI_ERROR` contains error code
	* info of length of msg

---
__Sample Code__

* __Sample code for *2nodes communication* in *[2nodes_msg_passing.c][code1_ref]*__

* __Sample code for *3 nodes communication* in *[3nodes_msg_communication][code2_ref]*__

	* `MPI_ANY_SOURCE` is used in 3nodes conmmunication resulting in unpredictable receiving order of msg

---
---

####2.Point-to-point communication with deadlock situation


__Program which deadlock depends on the MPI runtime__

    MPI_Comm_rank (comm, &my_rank);
    if (my_rank == 0){
    	MPI Send (sendbuf, count, MPI INT, 1, tag, comm);		MPI Recv (recvbuf, count, MPI INT, 1, tag, comm, 					&status);
    }else if (my_rank == 1){
    	MPI Send (sendbuf, count, MPI INT, 0, tag, comm);		MPI Recv (recvbuf, count, MPI INT, 0, tag, comm, 					&status);
    }

__Likely reasons for deadlock situation__

1. Miss construction of __MPI_Send__ and __MPI_recv__ operation
	* if two nodes send msg to each other, and both nodes execute __MPI_Recv()__ before __MPI_Send()__, this will cause a __WAITING__ deadlock for both nodes
	
2. Whether MPI runtime uses __Internal system buffer__ useage
	* MPI maintine __send_buffer__ and __recv_buffer__ for __send/recv__ operations
	* MPI runtime choose whether to use __system buffer__ (depends on the implementation of MPI)
	* If MPI use __system buffer__, after executing __MPI_Send()__ operation, message will be copied from __send_buffer__ to __system buffer__
	* Once __send_buffer__ is empty, MPI will use it for other send operation immediately _(or continue the execution of program)_. Therefore, the above code will __not cause deadlock__
	* If MPI does __not use system buffer__, the __only__ way to clear the __send_buffer__ is to receive the message
	* Therefore the above codes will cause __deadlock__


__Secure implementation of above code__

	MPI_Comm_rank (comm, &my_rank);
    if (my_rank == 0){
    	MPI Send (sendbuf, count, MPI INT, 1, tag, comm);		MPI Recv (recvbuf, count, MPI INT, 1, tag, comm, 					&status);
    }else if (my_rank == 1){		MPI Recv (recvbuf, count, MPI INT, 0, tag, comm, 					&status);
		MPI Send (sendbuf, count, MPI INT, 0, tag, comm);
    }
    
---



__Secure Implementation of Send/Recv message__ 

* For four processes:
	* Two phases logical ring communication is used
	* Even rank execute __send__, odd rank execute __recv__ in same phase
	*  e.g. for 3 processors, need 3 phases to complete ring communication



__MPI secure Send/Recv operation__

* *MPI runtime will guarantee deadlock freedom, also for no useage of system buffer situation*

* *__send_buff__ and __recv_buff__ must point to __disjoint__ memory locations*

__Code :__

	int MPI_Sendrecv (void *sendbuf,
						int sendcount,
						MPI_Datatype sendtype,
						int dest,
						int sendtag,
						void *recvbuf,
						int recvcount,
						MPI_Datatype recvtype,
						int source,
						int recvtag,
						MPI.Comm comm,
						MPI.Status *status)


---
---


####3.Non-blocking Operations & Communication Modes

*Aim is to fill the waiting time (of send/recv) with other operations e.g. overlapping __communication__ and __computation__*


[^1]: MPI Date Type:



[Amazon_book_link_ref]:http://www.amazon.com/s/ref=nb_sb_ss_c_0_25?url=search-alias%3Dstripbooks&field-keywords=parallel+programming+for+multicore+and+cluster+systems&sprefix=parallel+programming+for+multicore+and+cluster+systems%2Caps%2C348 "Book list on Amazon"

[mpi_docs_ref]:http://www.mpi-forum.org/docs/docs.html "MPI official documents reference"

[openmpi_ref]:http://www.open-mpi.org/ "openmpi opensource website"

[code1_ref]:https://github.com/rayzk/CS3210_Parallel_Programming_MPI/blob/master/2nodes_msg_passing.c

[code2_ref]:https://github.com/rayzk/CS3210_Parallel_Programming_MPI/blob/master/3nodes_msg_passing.c


