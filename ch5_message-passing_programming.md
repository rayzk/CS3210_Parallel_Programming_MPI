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






[^1]: data type of MPI


[Amazon_book_link_ref]:http://www.amazon.com/s/ref=nb_sb_ss_c_0_25?url=search-alias%3Dstripbooks&field-keywords=parallel+programming+for+multicore+and+cluster+systems&sprefix=parallel+programming+for+multicore+and+cluster+systems%2Caps%2C348 "Book list on Amazon"

[mpi_docs_ref]:http://www.mpi-forum.org/docs/docs.html "MPI official documents reference"

[openmpi_ref]:http://www.open-mpi.org/ "openmpi opensource website"

