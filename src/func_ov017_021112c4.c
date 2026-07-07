extern char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char* prev);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern int _ZN9Animation7AdvanceEv(char* t);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(char* t);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(char* t);
int func_ov017_021112c4(char* c) {
    if (*(unsigned char*)(c+0x338) == 0) {
        int ok = 1;
        char* p = _ZN5Actor15FindWithActorIDEjPS_(0xd, 0);
        while (p != 0) {
            int state = *(int*)(p+0x16c);
            int cond = 1;
            if (state != 1) {
                if (state != 2) cond = 0;
            }
            if (cond == 0) ok = 0;
            p = _ZN5Actor15FindWithActorIDEjPS_(0xd, p);
        }
        if (ok != 0) *(unsigned char*)(c+0x338) = 1;
    } else {
        int d = *(int*)(c+0x334) - *(int*)(c+0x60);
        if (d < 0) d = -d;
        if (d < 0x92e000) {
            int* q;
            *(int*)(c+0x33c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c+0x33c), 3, 0x96, c+0x74, 0);
            q = (int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
            *q -= 0x5000;
        }
    }
    *(int*)(c+0x32c) = 0x1000;
    _ZN9Animation7AdvanceEv(c+0x320);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
