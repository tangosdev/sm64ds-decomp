void _ZN18SolidHeapAllocator8ResetEndEv(void *c){
 int *node, *p=(int*)((char*)c+0x24);
 for(node=(int*)p[2];node;node=(int*)node[3]) node[2]=*(int*)((char*)c+0x1c);
 p[1]=*(int*)((char*)c+0x1c);}
