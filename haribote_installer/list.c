/*
	Name: list.h
	Copyright: 
	Author: Allen
	Date: 31/05/21 18:04
	Description: 
*/

#include "bootpack.h"


void list_init (struct list* list) {
   list->head.prev = NULL;
   list->head.next = &list->tail;
   list->tail.prev = &list->head;
   list->tail.next = NULL;
}

void list_insert_before(struct list_elem* before, struct list_elem* elem) { 

   before->prev->next = elem; 

   elem->prev = before->prev;
   elem->next = before;

   before->prev = elem;

}

void list_push(struct list* plist, struct list_elem* elem) {
   list_insert_before(plist->head.next, elem); 
}

void list_append(struct list* plist, struct list_elem* elem) {
   list_insert_before(&plist->tail, elem);     
}

void list_remove(struct list_elem* pelem) {
   
   pelem->prev->next = pelem->next;
   pelem->next->prev = pelem->prev;

}

struct list_elem* list_pop(struct list* plist) {
   struct list_elem* elem = plist->head.next;
   list_remove(elem);
   return elem;
} 

BOOL elem_find(struct list *plist, struct list_elem *obj_elem) {
   struct list_elem* elem = plist->head.next;
   while (elem != &plist->tail) {
      if (elem == obj_elem) {
	 		return TRUE;
      }
      elem = elem->next;
   }
   return FALSE;
}

struct list_elem* list_traversal(struct list* plist, function func, int arg) {
   struct list_elem* elem = plist->head.next;
   if (list_empty(plist)) { 
      return NULL;
   }

   while (elem != &plist->tail) {
      if (func(elem, arg)) {		  
	 		return elem;
      }					  
      elem = elem->next;	       
   }
   return NULL;
}

UINT list_len(struct list* plist) {
   struct list_elem* elem = plist->head.next;
   UINT length = 0;
   while (elem != &plist->tail) {
      length++; 
      elem = elem->next;
   }
   return length;
}

BOOL list_empty(struct list* plist) {		
   return (plist->head.next == &plist->tail ? TRUE : FALSE);
}
