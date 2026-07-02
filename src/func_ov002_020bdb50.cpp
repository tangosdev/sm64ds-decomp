//cpp
extern "C" {
extern void func_ov002_020bdc18(char* c);
extern void func_ov002_020dab14(char* c);
extern void func_ov002_020d71a0(char* p);
extern void func_ov002_020d5cec(void);
void func_ov002_020bdb50(char* c, int arg) {
    func_ov002_020bdc18(c);
    func_ov002_020dab14(c);
    if (arg != 0) return;
    if (*(int*)(c+0x360) == 0) return;
    func_ov002_020d71a0(c);
    {
        char* obj = *(char**)(c+0x360);
        char* vt = *(char**)obj;
        int r = (*(int(**)(char*))(vt+0x48))(obj);
        if (r == 0) return;
    }
    {
        char* obj = *(char**)(c+0x360);
        int b1 = (*(unsigned short*)(obj+0xc) == 0xbf);
        if (b1) {
            func_ov002_020d5cec();
            (*(unsigned short *)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFF)) &= ~2;
        } else {
            char* vt = *(char**)obj;
            (*(void(**)(char*, char*))(vt+0x4c))(obj, c);
        }
    }
    *(int*)(c+0x360) = 0;
}
}
