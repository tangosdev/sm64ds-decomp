extern int func_ov074_02121a20(void* c, int idx);
extern int func_ov074_021207b8(void* c);
extern void func_ov074_02121a4c(void* c, int i);
extern void func_02012694(int a, void* p);
extern int func_ov074_02120474(char* c);
extern int func_ov074_021204c0(void* c);
extern void func_ov074_021203e4(char* c, int i);

void func_ov074_0211fa74(char* c)
{
    char* a = c;
    int b;
    switch (*(unsigned char*)(a + 0x603)) {
    case 0:
        if (func_ov074_02121a20(a, 6) != 0) {
            unsigned char* p;
            b = func_ov074_021207b8(a);
            if (b == 0) return;
            p = (unsigned char*)(((int)a + 0x603));
            *p = *p + 1;
            func_ov074_02121a4c(a, 0);
            func_02012694(0x15c, a + 0x74);
            return;
        }
        {
            unsigned char* p = (unsigned char*)(((int)a + 0x603));
            *p = *p + 1;
        }
        func_ov074_02121a4c(a, 0);
        func_02012694(0x15c, a + 0x74);
        return;
    case 1:
        {
            b = func_ov074_02120474(a);
            if (func_ov074_021204c0(a) == 0) return;
            if (b == 0) return;
            func_ov074_021203e4(a, 2);
        }
        return;
    }
}
