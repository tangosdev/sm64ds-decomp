extern int _ZN8Platform21UpdateModelPosAndRotYEv(char* t);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(char* t);
extern char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char* prev);
extern int data_0209caa0[];
void func_ov012_0211123c(char* c) {
    int* q;
    char* p;
    if (*(unsigned char*)(c+0x31e)) return;
    q = (int*)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    *q -= 0x64000;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(unsigned char*)(c+0x31e) = 1;
    p = _ZN5Actor15FindWithActorIDEjPS_(0x22, 0);
    if (p == 0) return;
check:
    if (p != c) {
        if (*(unsigned char*)(p+0x31e)) {
            data_0209caa0[2] |= 0x80000;
        }
        return;
    }
    p = _ZN5Actor15FindWithActorIDEjPS_(0x22, p);
    if (p != 0) goto check;
}
