// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.


#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "machine.h"
#include "scheduler.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
//enum ProcStatus {JUST_CREATED, RUNNING, READY, BLOCKED}; 
typedef struct { 
  char* name;               // name of the program 
  ThreadStatus status;        // current status of the process 
} ProcInfo; 

typedef struct { 
  int totalticks ;             // Total time running Nachos 
  int idleticks  ;              // Time spent idle (no threads to run) 
  int systemticks ;            // Time spent executing system code 
  int usertick ;              // Time spent executing user code 
  int numprocs ;               // Total number of processes in processArray 
  ProcInfo *proc;             // Array of "numprocs" number of "ProcInfo" structures providing information about each process in the system 
} SysInfo; 





void CreatProcess(void* arg)
{
//	kernel->currentThread = (Thread*) arg;
	cout<< "Running Forked Thread" <<"\n";
	kernel->scheduler->Print();
	kernel->currentThread->space->Execute();
	
	ASSERTNOTREACHED();	
}


	
void Copy_Parent_Addrspace(Thread* ParentThread, Thread* Child)
{	
	unsigned int Start_parent,End_parent,current;
	Start_parent=(ParentThread->space->slot)*NumSlotPages*PageSize;
	End_parent=((ParentThread->space->slot)+1)*NumSlotPages*PageSize-1;
	current=(Child->space->slot)*NumSlotPages*PageSize;
		for(int i=Start_parent;i<=End_parent;i++)
		{
		kernel->machine->mainMemory[current]=kernel->machine->mainMemory[i];
		current++;
		}
		Child->space->numPages = ParentThread->space->numPages;
}

void Copy_Register(Thread* ParentThread, Thread* Child)
{
	
     ParentThread->SaveUserState();
    for (int i = 0; i < NumTotalRegs; i++)
	{	Child->userRegisters[i] = ParentThread->userRegisters[i];
	}
	
	Child->userRegisters[PrevPCReg]=Child->userRegisters[PCReg];
	Child->userRegisters[PCReg]=Child->userRegisters[PCReg]+4;
	Child->userRegisters[NextPCReg]=Child->userRegisters[PCReg]+4;
	for (int i = 0; i < MachineStateSize; i++)
    {Child->machineState[i]=ParentThread->machineState[i];
	}
	Child->userRegisters[2] = 0;
	
}
int print_state(int state)
{
	if (state==0) { cout<< "JUST_CREATED"<<"\n" ;}
	if (state==1) { cout<< "RUNNING "<<"\n" ;}
	if (state==2) { cout<< "READY"<<"\n" ;}
	if (state==3) { cout<< "BLOCKED"<<"\n" ;}

	}

void Info(SysInfo * info)
{

	
	ListIterator<Thread *> *temp;
	info->totalticks = kernel->stats->totalTicks;             // Total time running Nachos 
    info->idleticks = kernel->stats->idleTicks;              // Time spent idle (no threads to run) 
  info->systemticks = kernel->stats->systemTicks;            // Time spent executing system code 
  info->usertick = kernel->stats->userTicks;              // Time spent executing user code 
  info->numprocs = kernel->scheduler->readyList->NumInList()+1;               // Total number of processes in processArray 
	temp = new ListIterator<Thread *>(kernel->scheduler->readyList);
	info->proc = new ProcInfo[info->numprocs];
	cout<<"Number of Processes : "<<info->numprocs<<endl;
	int k = 0;
	while(temp->IsDone() == FALSE)
	{
		info->proc[k].name = temp->Item()->getName() ;
		info->proc[k].status = temp->Item()->status;
		
		cout << info->proc[k].name << " : ";
		print_state( info->proc[k].status ); 
		temp->Next();
		k++;
	}
	info->proc[k].name = kernel->currentThread->getName();
	info->proc[k].status = kernel->currentThread->status;
		cout << info->proc[k].name << " : ";
		print_state( info->proc[k].status );
			return;
}

void
ExceptionHandler(ExceptionType which)
{
	IntStatus old;
	char *filename=new char[100];
	int location,i,ch,processID,End_memory_loaction,start_memory_loaction;
    int type = kernel->machine->ReadRegister(2); // get the syscall code from r2
     
	Thread *mythread;
	AddrSpace *space;
	SysInfo * info;
	info = new SysInfo;

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) { //  case switching case type : 1) SyscallException
    case SyscallException:
      switch(type) { //case switching according to syscall
      
 //-------------------------------------------------------------------------
	  case SC_SysStats:
      cout<<"Exit system call by "<<kernel->currentThread->getName()<<"\n";		  
		Info(info);
		
	   /* set previous programm counter (debugging only)*/ 
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg)); // set priviousPC reg to Current PCreg

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4); //increment the PCreg
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	
	return;
	
	ASSERTNOTREACHED();

	break;      
 //------------------------------------------------------------------------    
      case  SC_Exit:
      kernel->scheduler->Print();
      cout<<"\n Exit system call by "<<kernel->currentThread->getName()<<"\n";		  
      old = kernel->interrupt->SetLevel(IntOff);
      mythread = kernel->scheduler->FindNextToRun();
      //kernel->interrupt->SetLevel(old);
	  if(mythread == NULL)
	  {
	  	SysHalt();
	  }
	  
	  
      //kernel->machine->WriteRegister(2, processID);
	  /* set previous programm counter (debugging only)*/ 
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg)); // set priviousPC reg to Current PCreg

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4); //increment the PCreg
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	
	kernel->scheduler->Run(mythread, TRUE);
    (void)kernel->interrupt->SetLevel(old);
	
	return;
	ASSERTNOTREACHED();
	break;      
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
    case SC_Fork:
    
      cout<< "Fork system call by "<<kernel->currentThread->getName()<<"\n";
      
	  space=new AddrSpace();			//Creating new address Space for child
      mythread=new Thread("Forked");	
      mythread->space=space;			//address spce assignment
      Copy_Parent_Addrspace(kernel->currentThread,mythread);	//copy parent Address Space Containt
	  Copy_Register(kernel->currentThread,mythread);			//Copy Parent Process Register

    mythread->Fork(&CreatProcess,(void*)mythread);				//Thread Frok (intialize stack and machine State)
//              kernel->scheduler->Print();
    kernel->machine->WriteRegister(2,1);	//returing  1 to parent
      
      /* set previous programm counter (debugging only)*/ 
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg)); // set priviousPC reg to Current PCreg

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4); //increment the PCreg
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	
	return;
	
	ASSERTNOTREACHED();

	break;      
	
//----------------------------------------------------------------------
//----------------------------------------------------------------------
      
      case  SC_Exec:
      cout<<"Exec system call by "<<kernel->currentThread->getName()<<"\n";
      location = kernel->machine->ReadRegister(4); 
      
      if(!kernel->machine->ReadMem(location,1,&ch))return;
      i=0;
      while((char)ch!='/0')
      {
		  filename[i]=(char)ch;
		  location+=1;
		  i++;
		  if(!kernel->machine->ReadMem(location,1,&ch))return;
	  }
	  filename[i]='\0';
		  
      
      
      //space =new AddrSpace();
      
     start_memory_loaction=(kernel->currentThread->space->slot*NumSlotPages*PageSize);
     End_memory_loaction=(kernel->currentThread->space->slot+1)*NumSlotPages*PageSize-1;
    // zero out the entire allocated address space for this process
    for(i=start_memory_loaction;i<=End_memory_loaction;i++)
    {kernel->machine->mainMemory[i]=0;}
    
      kernel->currentThread->space->Load(filename,kernel->currentThread->space);
      kernel->currentThread->space->Execute();
     
     
    

      kernel->machine->WriteRegister(2, processID);  
	  /* set previous programm counter (debugging only)*/ 
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg)); // set priviousPC reg to Current PCreg

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4); //increment the PCreg
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	

	
	return;
	
	ASSERTNOTREACHED();

	break;      
      
     
//----------------------------------------------------------------------------      
//----------------------------------------------------------------------------      

      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
	SysHalt(); // calling function defined in "ksyscall.h" 

	ASSERTNOTREACHED();
	break;
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------
      case SC_Add:
      cout<<"Add system call by "<<kernel->currentThread->getName()<<"\n";
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	/* Process SysAdd Systemcall*/
	int result; 
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5)); // calling SysAdd defined in ksyscall with argument present in r4 and r5

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);	//write result in r2
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/ 
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg)); // set priviousPC reg to Current PCreg

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4); //increment the PCreg
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;


      case SC_Mult:
	DEBUG(dbgSys, "Mult " << kernel->machine->ReadRegister(4) << " * " << kernel->machine->ReadRegister(5) << "\n");
	/* Process SysMult Systemcall*/
	result = SysMult(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5)); // calling SysAdd defined in ksyscall with argument present in r4 and r5

	DEBUG(dbgSys, "Mult returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);	//write result in r2
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/ 
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg)); // set priviousPC reg to Current PCreg

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4); //increment the PCreg
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;


     default:
	cerr << "Unexpected system call " << type << "\n";
		break;
      }
      
      break;
    default:
      cerr << "Unexpected user mode exception " << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}

