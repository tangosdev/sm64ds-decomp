    extern void func_ov063_02119c18(void* c, unsigned int id);
    extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void* pos, void* rot, int e, int f);
    
    void func_ov063_02116bf4(char* c)
    {
        unsigned short *ip = (unsigned short *)(((long long)(int)(c + 0x5d4)) & 0xFFFFFFFFFFFFFFFFLL);
        int *r3 = (int *)(((long long)(int)(c + 0x19c)) & 0xFFFFFFFFFFFFFFFFLL);
        unsigned char st;
    
        *ip = (unsigned short)(*ip & ~8);
        *r3 = *r3 | 1;
        func_ov063_02119c18(c, 0x9f);
    
        st = *(unsigned char*)(c + 0x5cc);
        switch (st) {
        case 0:
            if (*(int*)(c + 0x580) >= 0x3e8000)
                return;
            if (*(int*)(c + 0x5a0) < 5) {
                unsigned char cb = *(unsigned char*)(c + 0x5cb);
                if (cb != 5 && (int)cb - *(int*)(c + 0x5a0) < 2) {
                    char* p = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                        0xd1, 0xfff1, c + 0x5c, c + 0x92, *(signed char*)(c + 0x5d0), -1);
                    if (p != 0) {
                        *(int*)(p + 0x494) = *(int*)(c + 4);
                        *(int*)(p + 0x498) = *(int*)(c + 0x498);
                    }
                    {
                        unsigned char *q = (unsigned char *)(((long long)(int)(c + 0x5cb)) & 0xFFFFFFFFFFFFFFFFLL);
                        *q = *q + 1;
                    }
                }
                {
                    unsigned char *q = (unsigned char *)(((long long)(int)(c + 0x5cc)) & 0xFFFFFFFFFFFFFFFFLL);
                    *q = *q + 1;
                }
            }
            if (*(int*)(c + 0x5a0) >= 5)
                *(unsigned char*)(c + 0x5cc) = 2;
            break;
        case 1:
            if (*(unsigned short*)(c + 0x100) > 0x3c)
                *(unsigned char*)(c + 0x5cc) = 0;
            break;
        case 2:
            break;
        }
    }
