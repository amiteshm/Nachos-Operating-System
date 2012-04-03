// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "thread.h"

#define UserStackSize		1024 	// increase this as necessary!
#define NumSlot		4	//number of addrspace that can be given at a time
#define NumSlotPages	32	//number of Physical slote that a slot contain


extern int Slotarray[NumSlot]; //counter for total number of pages allocated till now

class AddrSpace {
  public:
    AddrSpace();
    ~AddrSpace();			// De-allocate an address space

    bool Load(char *fileName,Thread* myThread);		// Load a program into addr space from
                                        // a file
					// return false if not found

    void Execute();             	// Run a program
					// assumes the program has already
                                        // been loaded

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 

    // Translate virtual address _vaddr_
    // to physical address _paddr_. _mode_
    // is 0 for Read, 1 for Write.
    ExceptionType Translate(unsigned int vaddr, unsigned int *paddr, int mode);
    unsigned int Translate_Vitual_to_Phy(unsigned int Vaddr);
    int slot;	
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
	
    unsigned int numPages;		// Number of pages in the virtual 
	
  private:
					// address space
		//Slot assigned to this addrspace
    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code
};

#endif // ADDRSPACE_H
