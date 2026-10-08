extern void func_ov007_020b7d94(void *self);
extern void *data_ov007_0210342c;
extern int data_ov007_02103428;

void func_ov007_020b43dc(void *self) {
    void *obj = data_ov007_0210342c;
    data_ov007_02103428 = 1;
    void **chain1 = *(void***)((char*)obj + 0x16c);
    void *inner1 = chain1[0];
    void *elem1 = ((void**)inner1)[1];
    *(short*)((char*)elem1 + 2) = 3;
    void *obj2 = data_ov007_0210342c;
    void **chain2 = *(void***)((char*)obj2 + 0x170);
    void *inner2 = chain2[0];
    void *elem2 = ((void**)inner2)[1];
    *(short*)((char*)elem2 + 2) = 3;
    func_ov007_020b7d94(self);
}
