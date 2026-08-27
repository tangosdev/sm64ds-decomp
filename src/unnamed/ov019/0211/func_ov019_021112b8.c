struct S {
    char pad[0x37c];
    int f37c;
    char pad2[0x394-0x380];
    unsigned char f394;
};

void func_ov019_021112b8(struct S* c) {
    char* t;
    if (c->f394 != 0) return;
    t = *(char**)((char*)c + 0x378);
    if (*(unsigned char*)(t + 0x6de) != 0) {
        int a8 = *(int*)(t + 0xa8);
        int a0 = *(int*)(t + 0xa0);
        if (a8 > a0) return;
        {
            int* p = &c->f37c;
            int d = a8;
            if (d < 0) d = -d;
            *p = *p + d;
            if (c->f37c > 0x7d0000) c->f394 = 1;
        }
        return;
    }
    c->f37c = 0;
}
