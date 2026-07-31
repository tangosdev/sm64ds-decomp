extern void _ZN9Animation7AdvanceEv(void* self);
extern int _ZN9Animation8FinishedEv(void* self);
extern void func_0201267c(int a, void* p);

void func_ov079_02124530(char* c){
    _ZN9Animation7AdvanceEv(c + 0x31c);
    if (_ZN9Animation8FinishedEv(c + 0x31c)) {
        short* a = (short*)(c + 0x8c);
        if (*(unsigned char*)(c + 0x414)) { int* q = (int*)(((int)c + 0x3c8)); *q = *q + 0xe0; }
        else { int* q = (int*)(((int)c + 0x3c8)); *q = *q + 0x140; }
        *a = *a + *(int*)(c + 0x3c8);
        if (*(short*)(c + 0x8c) > 0x4000) {
            *(int*)(c + 0x3c8) = 0;
            *(short*)(c + 0x8c) = 0x4000;
            *(int*)(c + 0x3b0) = 5;
            func_0201267c(0xc6, c + 0x74);
        }
        if (*(unsigned char*)(c + 0x414)) {
            *(int*)(c + 0x410) = (short)*(short*)(c + 0x8c) * (short)0x15;
            if (*(int*)(c + 0x410) >= 0x53000) *(int*)(c + 0x410) = 0x53000;
        } else {
            *(int*)(c + 0x410) = (short)*(short*)(c + 0x8c) * (short)0xa;
            if (*(int*)(c + 0x410) >= 0x25000) *(int*)(c + 0x410) = 0x25000;
        }
        return;
    }
    {
        int* p = (int*)(((int)c + 0x60));
        *p = *p + ((*(unsigned char*)(c + 0x414) + 1) << 15);
    }
}
