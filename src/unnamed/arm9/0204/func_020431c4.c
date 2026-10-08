struct Node;
extern void *func_0203b358(struct Node *thiz);
extern void *func_0203b394(struct Node *thiz);
struct Sub { unsigned char pad[0xe]; unsigned char fe; };
struct Elem { unsigned char pad[0x10]; struct Sub *f10; };
int func_020431c4(void *thiz){
 struct Node *base = (struct Node *)((char *)thiz + 0x14);
 void *end = func_0203b358(base);
 struct Elem *node = *(struct Elem **)((char *)base + 4);
 int t;
 while (node != 0 && (void*)node != end) {
   t = !node->f10->fe;
   if (t != 0) return 1;
   node = (struct Elem*)func_0203b394((struct Node*)node);
 }
 return 0;
}
