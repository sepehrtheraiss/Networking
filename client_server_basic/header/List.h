// ---------------------------------------------------------------
// Sepehr Raissian
// Sraissia@ucsc.edu
// CMPS101 pa4
// List.h
// ----------------------------------------------------------------
//-----------------------------------------------------------------------------
// List.h
// Header file for List ADT
//-----------------------------------------------------------------------------

#ifndef _LIST_H_INCLUDE_
#define _LIST_H_INCLUDE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Exported type --------------------------------------------------------------
typedef struct ListObj* List;

// Constructors-Destructors ---------------------------------------------------

// newList()
// Returns reference to new empty List object.
List newList(void);

// freeList()
// pre: *pL != NULL, pL != NULL
// Frees all heap memory associated with List *pL, and sets *pL to NULL.
void freeList(List* pL);

// Access functions -----------------------------------------------------------

// length()
// pre: L != NULL
// Returns the length of L.
int length(List L);

//index()
// pre: L != NULL
// If cursor is defined, returns the index of the cursor element,
// otherwise returns -1.
int index(List L);

// front()
// Pre: L != NULL, length()>0
// Returns front element.
char* front(List L);

// back()
// Pre: L != NULL, length()>0
// Returns back element.
char* back(List L);

// get()
// Pre: L != NULL, length()>0, index()>=0
// Returns cursor element.
char* get(List L);
// returns the length of the string
int getLength(List l);
// writes out everything to the given fd
int writeOut(List l,int fd);
// equals()
// pre: L != NULL
// Returns true if this List and L are the same integer
// sequence. The cursor is ignored in both lists.
int equals(List L,List C);

// Manipulation procedures ----------------------------------------------------

// clear()
// pre: L != NULL
// Resets this List to its original empty state.
void clear(List L);

// moveFront()
// pre: L != NULL
// If List is non-empty, places the cursor under the front element,
// otherwise does nothing.
void moveFront(List L);

// moveBack()
// pre: L != NULL
// If List is non-empty, places the cursor under the back element,
// otherwise does nothing.
void moveBack(List L);

// movePrev()
// pre: L != NULL
// If cursor is defined and not at front, moves cursor one step toward
// front of this List, if cursor is defined and at front, cursor becomes
// undefined, if cursor is undefined does nothing.
void movePrev(List L);

// moveNext()
// pre: L != NULL
// If cursor is defined and not at back, moves cursor one step toward
// back of this List, if cursor is defined and at back, cursor becomes
// undefined, if cursor is undefined does nothing.
void moveNext(List L);

// prepend()
// pre: L != NULL
// Insert new element into this List. If List is non-empty,
// insertion takes place before front element.
void prepend(List L,char* data);

// append()
// pre: L != NULL
// Insert new element into this List. If List is non-empty,
// insertion takes place after back element.
void append(List L,char* data,int l);

// insertBefore()
// Pre: L != NULL,length()>0, index()>=0
// Insert new element before cursor.
void insertBefore(List L,char* data);

// insertAfter()
// Pre: L != NULL,length()>0, index()>=0
// Inserts new element after cursor.
void insertAfter(List L,char* data);

// deleteFront()
// Pre: L != NULL,length()>0
// Deletes the front element.
void deleteFront(List L);

// deleteBack()
// Pre: L != NULL, length()>0
// Deletes the back element.
void deleteBack(List L);

// delete()
// Pre: L != NULL, length()>0, index()>=0
// Deletes cursor element, making cursor undefined.
void delete(List L);

// Other operations -----------------------------------------------------------

// printList()
// pre: L != NULL
// Prints data elements in L on a single line to stdout.
void printList(FILE* out, List L);

// copy()
// pre: L != NULL
// Returns a new List representing the same integer sequence as this
// List. The cursor in the new list is undefined, regardless of the
// state of the cursor in this List. This List is unchanged.
List copyList(List L);

#endif
