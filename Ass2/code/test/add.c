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
	int i=0;
	for(i=1;i<100;i++){
		SysPrint("this is add ");
			Write(i);
			}
 // Exit(0);
}
