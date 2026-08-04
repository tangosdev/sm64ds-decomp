//cpp
extern "C" {
extern int _ZN9Animation7AdvanceEv(void *);
extern int func_ov006_020c35e8(void *);
void func_ov006_020c3d18(char *c)
{
    int v = *(int*)(c+4);
    void *p = c + (v >> 1);
    int *fn;
    if (v & 1) {
        int *vt = *(int**)p;
        int off = *(int*)c;
        fn = (int*)*(int*)((char*)vt + off);
    } else {
        fn = (int*)*(int*)c;
    }
    ((void(*)(void*))fn)(p);
    _ZN9Animation7AdvanceEv(c + 0xd68);
    int i = 0;
    char *e = c + 8;
    do {
        if (*(int*)(c+0x48) != 0)
            func_ov006_020c35e8(e);
        i++;
        c += 0x98;
        e += 0x98;
    } while (i < 0x16);
}
}
