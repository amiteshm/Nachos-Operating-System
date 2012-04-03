/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "addrspace.h"
#include "scheduler.h"
#include "thread.h"
#include "synch.h"

int ShareMem(int size){
		return (kernel->currentThread->Attach(size));
}

void ShareClose(){
	kernel->currentThread->Detach();
}

/*int Trans(int Vaddr){
	int physAddr;
	kernel->machine->Translate(Vaddr,&physAddr,sizeof(int),TRUE);
	return physAddr;
}*/

void
Run_thread(void* t)
{
	Thread* Thr;
	Thr = (Thread*)t;
	kernel->currentThread->space = Thr->space;
	Thr->RestoreUserState();
	Thr->space->RestoreState();
	kernel->machine->Run();
}

void Sleep(int sleep_time){
	kernel->currentThread->SleepTest(sleep_time);
}

void getStatus(ThreadStatus i)
{
char stat[20];
	if (i==0) { strcpy(stat," JUST_CREATED");}
	if (i==1) { strcpy(stat," RUNNING");}
	if (i==2) { strcpy(stat," READY");}
	if (i==3) { strcpy(stat," BLOCKED");}
	cout << stat <<"\n";
}
void PrintStats()
{
	int i;
		cout << "*****************************************************************************************************\n";
	cout<< "Total ticks :" << kernel->stats->totalTicks << "\n";
	cout<< "Idle ticks :" << kernel->stats->idleTicks << "\n";
	cout<< "System ticks :" << kernel->stats->systemTicks << "\n";
	cout<< "User ticks :" << kernel->stats->userTicks << "\n";
	cout<< "Number of processes on ready list:" << kernel->scheduler->readyList->NumInList() + 1 << "\n";
	ListElement<Thread *> *proc;
	proc = kernel->scheduler->readyList->first;
	cout<<"\nNumber of processes are\n"<<"process-"<<kernel->currentThread->getName()<<": ";getStatus(kernel->currentThread->getstatus());
	for(i=0;i<(kernel->scheduler->readyList->NumInList());i++)
	{
			cout<<"process-"<<proc->item->getName()<<": ";getStatus(proc->item->getstatus());
			proc = proc->next;
	}
	cout << "*****************************************************************************************************\n";
	return;
}


void SysWrite(int addr)		// called from exception handler for Write system call.
{
	int val=addr;
	cout<< "We are writing-'" << val << "'" <<endl;
}

void SysHalt()
{
  kernel->interrupt->Halt();
}


void Disp(int var){								// Displays a meassage
int i=0,str=1;char strn[100];
	while(str){
	kernel->machine->ReadMem(var+i,1,&str);
	*(strn+(i++))=str;
	}
	cout << strn << "\n";
	return;
}





void Create_thread(int priority)			
{    
	Thread *t;
	t = new Thread("hello");
	AddrSpace *MySpace = new AddrSpace();
	t->space = MySpace;
	t->priority = priority;
	int j;
	for(j=0;j<(PageSize*No_pr);j++) kernel->machine->mainMemory[((MySpace->Epage)*PageSize)+j] = kernel->machine->mainMemory[( kernel->machine->pageTable[0].physicalPage ) * PageSize +j];
	if(kernel->currentThread->SharedMemStatus==1){
		t->space->pageTable[No_pr-1]=kernel->currentThread->space->pageTable[No_pr-1];
		t->SharedMemStatus=1;
	}
	t->SaveUserState();
	t->userRegisters[PCReg] += 4;
	t->userRegisters[PrevPCReg] += 4;
	t->userRegisters[NextPCReg] += 4;
	t->userRegisters[2] = 0;	
	t->Fork( (VoidFunctionPtr) Run_thread, t);

	return ;
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}



void Execute(int addr,char *prog)
{	int i=1,j = 0;
	while(i)
	{
		kernel->machine->ReadMem(addr+j,1,&i);
		*(prog+(j++)) = i;
	}
	AddrSpace *MySpace = kernel->currentThread->space;
	MySpace->Load(prog);
	MySpace->Execute();
}

int getExp(){
	int* k;
	k=&kernel->exp;
	return (int)k;
}

void setExp(int val,int point){
	int *k;
	k=(int *)point;
	*k=val;
	cout<<"Experimental value is "<<kernel->exp<<"\n";
}

int sem_create(int id){
	Semaphore *mySemaphore;
	mySemaphore = new Semaphore(id,1);
	return (int) mySemaphore;
}

void sem_wait(int sem){
	Semaphore *mySemaphore;
	mySemaphore = (Semaphore *) sem;
	mySemaphore->P();
	return;
}

void sem_signal(int sem){
	Semaphore *mySemaphore;
	mySemaphore = (Semaphore *) sem;
	mySemaphore->V();
}

void sem_destroy(int sem){
	Semaphore *mySemaphore;
	mySemaphore = (Semaphore *) sem;
	delete mySemaphore;
}

int CheckSem(int sem){
	Semaphore *mySemaphore;
	mySemaphore = (Semaphore *) sem;
	return mySemaphore->getValue();
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
