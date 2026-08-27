struct Node { char pad[0x4b4]; struct Node* next; };
extern struct Node* data_020a9d2c;

void func_020684d0(struct Node* n){
  struct Node* p = data_020a9d2c;
  if (p == 0) {
    data_020a9d2c = n;
  } else {
    while (p->next != 0) {
      p = p->next;
    }
    p->next = n;
  }
  n->next = 0;
}
