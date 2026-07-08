// NONMATCHING: different op / idiom (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern char LEVEL_FILE[];
extern int LEVEL_MODEL_DATA;
extern void _ZN5Model14LoadAndSetFileEtii(char* model, unsigned short id, int b, int c);
void _ZN5Stage9LoadModelEv(char* self) {
    _ZN5Model14LoadAndSetFileEtii(self + 0x86c, *(unsigned short*)(*(char**)LEVEL_FILE + 8), 1, -1);
    unsigned int i = 0;
    char* p = self + 0x874;
    char* node = ((char**)p)[1];
    unsigned int count = *(unsigned int*)(((char**)p)[0] + 0x24);
    while (i < count) {
        unsigned int v = *(unsigned int*)(node + 0x24);
        i++;
        if ((v & 0x1f0000) != 0x1f0000) {
            *(unsigned int*)(node + 0x24) = (v & ~0x3f000000) | 0x13000000;
        }
        node += 0x30;
    }
    LEVEL_MODEL_DATA = (int)p;
}
