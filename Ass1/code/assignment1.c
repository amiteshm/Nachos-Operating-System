
#include "syscall.h"

int
main()
{
  int result,pid,i=0;
  char* progs[] = {"add","\0"};
  while(progs[i]!= "\0"){
	  pid=Fork();
	  if(pid==0)	Exec("add");
	  else result= Add(3,4);
	  i++;
	  }
   SysStats();
  Exit(0);
  /* not reached */
}
