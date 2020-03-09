#ifndef NODE_H__
#include <stdio.h>
#include <stdlib.h>

struct Node{
  int data;
  struct Node *prev;
  struct Node *next;
  void (*init)();//function pointer for init node
};

void initNode(struct Node *self){//init node to point nothing.
  self->prev = NULL;
  self->next = NULL;
  self->data = 0;
}

#endif
