struct Node { struct Node* next; struct Node* prev; };
struct List { struct Node* head; struct Node* tail; };
int func_0203b27c(struct List* l, struct Node* n){
  if(n == 0) goto fail;
  if(n->next != 0){
    n->next->prev = n->prev;
  } else {
    if(n == l->head) l->head = n->prev;
  }
  if(n->prev != 0){
    n->prev->next = n->next;
  } else {
    if(n == l->tail) l->tail = n->next;
  }
  n->next = 0;
  n->prev = 0;
  goto ok;
fail:
  return 0;
ok:
  return 1;
}
