// NONMATCHING: extra logic (you do more) (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int _ZN8Platform21UpdateModelPosAndRotYEv(char* t);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(char* t);
extern char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char* prev);
extern int SAVE_DATA[];
void func_ov012_0211123c(char* c) {
    int* q;
    char* p;
    if (*(unsigned char*)(c+0x31e)) return;
    q = (int*)(c+0x60);
    *q -= 0x64000;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(unsigned char*)(c+0x31e) = 1;
    p = _ZN5Actor15FindWithActorIDEjPS_(0x22, 0);
    if (p == 0) return;
check:
    if (p != c) {
        if (*(unsigned char*)(p+0x31e)) {
            SAVE_DATA[2] |= 0x80000;
        }
        return;
    }
    p = _ZN5Actor15FindWithActorIDEjPS_(0x22, p);
    if (p != 0) goto check;
}
