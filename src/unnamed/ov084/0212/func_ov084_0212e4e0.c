extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void* prev);

int func_ov084_0212e4e0(char* c)
{
    void* p;
    if (*(int*)(c + 0x1e8) == 0) {
        *(signed char*)(c + 0x221) = -1;
        *(int*)(c + 0x1f0) = *(int*)(c + 4);
        *(int*)(c + 0x1f4) = *(int*)(c + 4);
        *(int*)(c + 0x1e8) = 1;
        p = 0;
        for (;;) {
            p = _ZN8dActor_c15FindWithActorIDEjPS_(0xfb, p);
            if (p == 0) break;
            if (p != c) {
                *(int*)((char*)p + 0x1e8) = 2;
                *(int*)((char*)p + 0x1f0) = *(int*)(c + 4);
            }
        }
    }
    *(int*)(c + 0x1ec) = 1;
    return 1;
}
