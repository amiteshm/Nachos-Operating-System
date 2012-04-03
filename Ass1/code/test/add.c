/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int
main()
{
  int result,pid;
  
  result = Add(42, 23);
  //pid = Fork();
   //SysStats();
    //char* progs[] = {"add","\0"};
  //int i;
  //while(progs[i]!="\0"){
  //if(pid==0)	Exec("halt");
  //else	result = Add(33,55);
   //SysStats();
  Exit(0);
  /* not reached */
}
