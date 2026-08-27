struct Node {
  struct Node* next;
  char pad58[0x58 - 4];
  int f58;
  int f5c;
  char pad7c[0x7c - 0x60];
  int f7c;
};
void func_02041ce0(struct Node* node, int arg1, int arg2){
  node = *(struct Node**)((char*)node + 0x2704);
  if (node == 0) return;
  do {
    if (node->f7c == 2 && arg1 == node->f58 && arg2 == node->f5c - node->f58) return;
    node = node->next;
  } while (node != 0);
}
