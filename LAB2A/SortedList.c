/*
 * SortedList (and SortedListElement)
 *
 *  A doubly linked list, kept sorted by a specified key.
 *  This structure is used for a list head, and each element
 *  of the list begins with this structure.
 *
 *  The list head is in the list, and an empty list contains
 *  only a list head.  The list head is also recognizable because
 *  it has a NULL key pointer.
 */

#include <sched.h>
#include <stdlib.h>
#include <string.h>

#include "SortedList.h"

int opt_yield = 0;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{

  if (!list || !element){
    return;
  }

  if (!(list->next)){

    if (opt_yield & INSERT_YIELD){
	sched_yield();
    }
      list->next = element;
      element->prev = list;
      element->next = NULL;
      return;
    }

  SortedListElement_t *temp = list->next;
  SortedListElement_t *p = list;

  while (temp && strcmp(element->key, temp->key) >= 0){
      p = temp;
      temp = temp->next;
    }


  if (opt_yield & INSERT_YIELD){
    sched_yield();
  }

  if (!temp){
      p->next = element;
      element->prev = p;
      element->next = NULL;
    }
  else{
      p->next = element;
      temp->prev = element;
      element->next = temp;
      element->prev = p;
    }

}

int SortedList_delete(SortedListElement_t *element)
{
  if (!element)
    return 1;

  if (opt_yield & DELETE_YIELD)
    sched_yield();

  if (element->next){
      if ((element->next)->prev != element)
	return 1;
      else
	(element->next)->prev = element->prev;
    }

  if (element->prev){
      if ((element->prev)->next != element)
	return 1;
      else
	(element->prev)->next = element->next;
    }

  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
  if (!list)
    return NULL;

  SortedListElement_t *p = list->next;

  while (p){
      if (opt_yield & LOOKUP_YIELD)
	sched_yield();

      if (strcmp(key, p->key) == 0)
	return p;

      p = p->next; 
    }
  return NULL;
}

int SortedList_length(SortedList_t *list)
{

  if (!list)
    return -1;

  int size = 0;
  SortedListElement_t *p = list->next;


  while (p){
      if (opt_yield & LOOKUP_YIELD)
	sched_yield();

      if (p->prev != NULL && (p->prev)->next != p)
	return -1;

      if (p->next != NULL && (p->next)->prev != p)
	return -1;

      size++;        
      p = p->next;    
    }
  return size;
}
