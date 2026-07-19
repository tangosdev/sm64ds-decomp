//cpp
extern "C" {
extern char data_0209f340[];
extern int data_0209f320;
void _ZN5Model14LoadAndSetFileEtii(char* model, unsigned short id, int b, int c);
void _ZN5Stage9LoadModelEv(char* self);
}
void _ZN5Stage9LoadModelEv(char* self) {
    _ZN5Model14LoadAndSetFileEtii(self + 0x86c, *(unsigned short*)(*(char**)data_0209f340 + 8), 1, -1);
    char* list;
    unsigned int count;
    char* node;
    unsigned int i;
    char* p;

    p = (char*)(((long long)(int)(self + 0x874)) & 0xFFFFFFFFFFFFFFFFLL);
    list = ((char**)p)[0];
    node = ((char**)p)[1];
    count = *(unsigned int*)(list + 0x24);
    i = 0;
    while (i < count) {
        unsigned int v = *(unsigned int*)(node + 0x24);
        i++;
        if ((v & 0x1f0000) != 0x1f0000) {
            *(unsigned int*)(node + 0x24) = (v & ~0x3f000000) | 0x13000000;
        }
        node += 0x30;
    }
    data_0209f320 = (int)p;
}
