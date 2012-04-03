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
#include "kernel.h"
#include "thread.h"

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

void
ExceptionHandler(ExceptionType which)
{
	char a[30];						// for Exec handler.
	int variable;					// for fork handler.
    int type = kernel->machine->ReadRegister(2);
  

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

	case SC_Add:
		
		int result;
		result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
				/* int op2 */(int)kernel->machine->ReadRegister(5));
		DEBUG(dbgSys, "Add returning with " << result << endl);
		DEBUG(dbgSys, result << "\n");
		/* Prepare Result */
		kernel->machine->WriteRegister(2, (int)result);
		
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;		
		case SC_Sleep:				
		Sleep(kernel->machine->ReadRegister(4));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_ShareMem:				
		kernel->machine->WriteRegister(2,ShareMem(kernel->machine->ReadRegister(4)));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_CheckSem:				
		kernel->machine->WriteRegister(2,CheckSem(kernel->machine->ReadRegister(4)));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
		
	case SC_SemCreate:				
		kernel->machine->WriteRegister(2,sem_create(kernel->machine->ReadRegister(4)));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_SemWait:				
		sem_wait(kernel->machine->ReadRegister(4));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
		case SC_SemSignal:				
		sem_signal(kernel->machine->ReadRegister(4));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_SemDestroy:				
		sem_destroy(kernel->machine->ReadRegister(4));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_SE:				
		setExp(kernel->machine->ReadRegister(4),kernel->machine->ReadRegister(5));
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_GE:				
		kernel->machine->WriteRegister(2,getExp());
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
//	case SC_Trans:				
	//	kernel->machine->WriteRegister(2,Trans(kernel->machine->ReadRegister(4)));
		/* Modify return point */
		//{
		  /* set previous programm counter (debugging only)*/
		 // kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		 // kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		 // kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		//}
		//return;
		
	case SC_ShareClose:				
		ShareClose();
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_SysStats:

		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		PrintStats();
		return;
		break;	
	case SC_Write:
		cout << "############################### : " <<kernel->machine->ReadRegister(4) << "\n";
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;	
	case SC_Exec:				
		Execute(kernel->machine->ReadRegister(4),a);
		/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_fork:
		Create_thread(kernel->machine->ReadRegister(4));
		kernel->machine->WriteRegister(2,1);

	/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_Print:
		Disp(kernel->machine->ReadRegister(4));
	/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		return;
		break;
	case SC_Exit:
	/* Modify return point */
		{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
		}
		cout << "------------------------------------------ Exit system call.\n";
		if(!kernel->machine->ReadRegister(4))
		{
		//	if(!kernel->scheduler->is_empty())
		//	{	
				ShareClose();
				kernel->currentThread->Finish();					
		//	}
		/*	else
			{
				SysHalt();
			}*/
		}
		else{SysHalt();}
		
		
		return;
	
	ASSERTNOTREACHED();

	break;

      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
      case SharedMemException:
      cerr << "Shared memory limit exceeded\n";
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
    ASSERTNOTREACHED();
}
