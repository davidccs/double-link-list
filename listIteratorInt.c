/*
  listIteratorInt.c ... list Iterator ADT implementation
  Written by David Chen
  Date: 14/3/2017
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "listIteratorInt.h"

#define TRUE 1
#define FALSE 0

typedef struct node *Node;

// doubly link list
typedef struct node {
    int value; // stores value inside
    Node next; // next pointer
    Node prev; // previous pointer
} node;


typedef struct IteratorIntRep {
    Node before; // previous cursor
    Node after; // next cursor
    Node reset; // keeps track of the start of the list
    Node lastAccessed; // keeps track of the last value accessed
    int callAccessed; // checks add, set, delete have been accessed
} IteratorIntRep;


static Node newNode(int data);

// creates a new node
Node newNode(int data){
    Node new = malloc(sizeof(struct node));
    assert(new != NULL);
    new->next = NULL;
    new->prev = NULL;
    new->value = data;
    return new;
 }

//Creates a new list iterator
IteratorInt IteratorIntNew(){

    IteratorInt new = malloc(sizeof(struct IteratorIntRep));
    assert (new != NULL);
    new->before = NULL;
    new->after = NULL;
    new->reset = NULL;
    new->lastAccessed = NULL;
    new->callAccessed = FALSE;

    return new;
}
//Resets it to the start of the list.
void reset(IteratorInt it){

  Node temp = it->before;
  // this case is only if the list is NOT empty
  // sets it to the beginning
  if (it->before != NULL){
      while (temp->prev != NULL){
          temp = temp->prev;
      }
      it->after = temp;
      it->before = NULL;
  } else {
      it->before = NULL;
      it->after = it->reset;
  }
}

int add(IteratorInt it, int v){

    Node new;
    Node temp;
    Node hold;
    new = newNode(v);
    int fact = FALSE;

    // if list is empty
    if (it->after == NULL && it->before == NULL){
        it->after = NULL;
        it->before = new;
        it->reset = new; // points to the first node created
        fact = TRUE; //
    // insert at the end
    } else if (it->after == NULL){
        temp = it->before;
        temp->next = new;
        new->prev = temp;
        it->before = new;
        it->after = NULL;
        fact = TRUE;
    // inserting at the front
    } else if (it->before == NULL){
        temp = it->after;
        new->next = temp;
        temp->prev = new;
        it->before = new;
        it->after = new->next;
        it->reset = new;
        fact = TRUE;
    // inserting inbetwen nodes
    } else {
        temp = it->before;
        hold = it->after;
        temp->next = new;
        new->prev = temp;
        hold->prev = new;
        new->next = hold;
        it->before = new;
        fact = TRUE;
    }
    it->callAccessed = FALSE;
    it->lastAccessed = new;

    return fact;
}

int hasNext(IteratorInt it){
  int fact = FALSE;
  // checks if after pointer is NULL
  if (it->after != NULL){
      fact = TRUE;
  } else {
      fact = FALSE;
  }
  return fact;
}

int hasPrevious(IteratorInt it){

  int fact = 0;
  // checks if previous pointer is NULL
  if (it->before != NULL){
      fact = TRUE;
  } else {
      fact = FALSE;
  }
  return fact;
}

int *next(IteratorInt it){

  int *ptr = NULL;
  // check if next node is not NULL
  if (it->after != NULL ){
      ptr = &(it->after->value);
      it->before = it->after;
      it->after = it->after->next;
      it->callAccessed = TRUE;
  // if the after node IS NULL
  } else {
      it->callAccessed = FALSE;
      ptr = NULL;
  }
  // save the returned value
  if (ptr != NULL) it->lastAccessed = it->before;
  return ptr;
}

int *previous(IteratorInt it){

    int *ptr = NULL;
    if (it->before != NULL){
        ptr = &(it->before->value);
        it->after = it->before;
        it->before = it->before->prev;
        it->callAccessed = TRUE;
    // before pointer DOES point at NULL
    } else {
        it->callAccessed = FALSE;
        ptr = NULL;
    }
    // save the returned value
    if (ptr != NULL) it->lastAccessed = it->after;
    return ptr;
}


int delete(IteratorInt it){

    int fact = FALSE;
    // checks next,previous has been successful
    if (it->callAccessed == TRUE){
        Node temp = it->lastAccessed;
        // empty list
        if (it->lastAccessed == NULL){
            fact = FALSE;
        // deleting one node
        } else if (it->lastAccessed->next == NULL && it->lastAccessed->prev == NULL){
            it->before = NULL;
            it->after = NULL;;
            it->reset = NULL;
            free(temp);
            //it->lastAccessed = NULL;
        // deleting node at the front of the list
        } else if (it->lastAccessed->next != NULL && it->lastAccessed->prev == NULL){
            it->reset = it->lastAccessed->next;
            it->after = it->lastAccessed->next;
            Node point = it->after;
            point->prev = NULL;
            it->before = NULL;
            free(temp);
            //it->lastAccessed = NULL;
        // deleting at the end of list
        } else if (it->lastAccessed->next == NULL && it->lastAccessed->prev != NULL){
            it->before = it->lastAccessed->prev;
            Node point = it->before;
            it->after = NULL;
            point->next = NULL;
            free(temp);
            //it->lastAccessed = NULL;
        // deleting inbetween
        } else {
            Node tempPrev = it->lastAccessed->prev;
            Node tempNext = it->lastAccessed->next;
            tempPrev->next = tempNext;
            tempNext->prev = tempPrev;
            it->after = tempNext;
            it->before = tempPrev;
            free(temp);
            //it->lastAccessed = NULL;
        }
        fact = TRUE;
    } else {
        fact = FALSE;
    }
    it->callAccessed = FALSE;
    return fact;
}

int set(IteratorInt it, int v){

  int fact = FALSE;
  // checks if next/prev && set is correct
  // also fix up reset function
  if (it->callAccessed == TRUE){
        it->lastAccessed->value = v;
        fact = TRUE;
  } else {
      fact = FALSE;
  }
  // ensures delete/set cannot be called CONSECUTIVELY
  it->callAccessed = FALSE;
  return fact;
}

int *findNext(IteratorInt it, int v){

    int *ptr = NULL;
    if (it->after != NULL){
        // searches for a node equal to v
        while (it->lastAccessed->value != v && it->lastAccessed->next != NULL){
            it->lastAccessed = it->lastAccessed->next;
        }
        it->after = it->lastAccessed->next;
        it->before = it->lastAccessed;
        it->callAccessed = TRUE;
        // v does not match
    } else {
        it->callAccessed = FALSE;
        ptr = NULL;
    }
    // resets call
    if (ptr != NULL) it->lastAccessed = it->before;

    return ptr;
}

int *findPrevious(IteratorInt it, int v){

    int *ptr = NULL;
    if (it->before != NULL){
    // searches for a node equal to v
        while (it->lastAccessed->value != v && it->lastAccessed->prev != NULL){
            it->lastAccessed = it->lastAccessed->prev;
        }
        it->after = it->lastAccessed;
        it->before = it->lastAccessed->prev;
        it->callAccessed = TRUE;
    // if v does not match
    } else {
        it->callAccessed = FALSE;
        ptr = NULL;
    }
    // resets the calls
    if (ptr != NULL) it->lastAccessed = it->after;

    return ptr;
}
