#include "syscall.h"

int DP(int i,int Chopstick1,int Chopstick2)
{int j=5;
	while(j){
		SysPrint("Process");
		Write(i);
		SysPrint("is Hungry");
		
		semaphore_wait(Chopstick1);
		if(check_semaphore(Chopstick2)){
		semaphore_wait(Chopstick2);}
		else
		{semaphore_signal(Chopstick1);}
		//
		SysPrint("Process");
		Write(i);
		SysPrint("is eating");
		//
	
		semaphore_signal(Chopstick1);
		SysPrint("Eating Over");
		semaphore_signal(Chopstick2);
	
		//
		SysPrint("Process");
		Write(i);
		SysPrint("is Thinking");
	
		sleep((i+1)*10);
		j--;
	}
Exit(0);
}

int main()
{
  int pid,i=0;
  int Chopstick[3];
 
  for(i=0;i<3;i++){
	 Chopstick[i]=semaphore_create(i);  
  }
	 
  for (i=0;i<3;i++){
	  pid=fork(5);
	  if(pid==0)	DP(i,Chopstick[i],Chopstick[(i+1)%3]);
  }
  Exit(0);
  /* not reached */
}
