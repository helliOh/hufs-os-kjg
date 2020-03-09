#ifndef DBL_LIST_H__
#include "node.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct{//typedef won't work because recursive definition won't work for the anonymous structure
  struct Node base;
  struct Node *currNode;
  int freeNodes;//free space for a list
  void (*init)();//function pointers
  int (*insert)();
  int (*remove)();
  int (*freeSpace)();
}dblList;

int insertNode (dblList *self, int data){//insert node into the list
  if(self->freeNodes != 0){
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->init = initNode;
    newNode->init(newNode);
    newNode->data = data;
    newNode->prev = self->currNode;
    self->currNode->next = newNode;
    self->currNode = newNode;
    (self->freeNodes)--;
    return data;
  }//ignored requests are to be handled by pthread_cond_wait()
  else {
    printf("buffer full!\n");
    return -1;
  }


}

int removeNode (dblList *self){//w ill never concat the nodes because there is no possibility to remove the node between others
  if(self->freeNodes == 100){
    printf("buffer empty!\n");
    return -1;//ifnore the remove requests if there is no data in the list
  }
  else{
    struct Node *targetNode = self->currNode;//this pointer will move to first node in order to find the proper target to remove
    while(targetNode != self->base.next)
      targetNode = targetNode->prev;
    int data = targetNode->data;//ignored requests are to be handled by pthread_cond_wait()
    if(targetNode->next == NULL){
      self->currNode = &(self->base);
      self->base.next = NULL;
    }
    else{
      targetNode->next->prev = &(self->base);
      self->base.next = targetNode->next;
    }
    free(targetNode);
    (self->freeNodes)++;
    return data;
  }
}

int freeSpace(dblList *self){
  return self->freeNodes;
}

void initList(dblList *self){
  self->base.init = initNode;//base node init, initNode() defined in node header file
  self->base.init();
  self->base.prev=NULL;

  self->currNode = &(self->base);//list init
  self->freeNodes = 100;//assume that there is only 100 nodes in the list
  self->insert = insertNode;//assign each function to the proper function pointer in list class
  self->remove = removeNode;
  self->freeSpace = freeSpace;
}

#endif
