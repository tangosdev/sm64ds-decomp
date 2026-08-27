extern void func_ov007_020c6d90(char* self, int idx);

struct S { int f0; int f4; char pad8[0x18]; int f20; int f24; int f28; int f2c; };
extern struct S *func_ov007_020c2a04(struct S *src, int start, int end);
extern void func_ov007_020c281c(char* self, int arg1);
extern void func_ov007_020c27b4(struct S *s);
extern void func_ov007_020c26fc(char* c);
extern void func_ov007_020c6e68(char* self, int i);

void func_ov007_020c6d20(char* self, int idx)
{
    struct S *src;
    struct S *d;
    int check;

    func_ov007_020c6d90(self, idx);

    src = *(struct S**)(self + 0x20);
    (*(struct S***)(self + 0x38))[idx] = func_ov007_020c2a04(src, 0, *(unsigned short*)((char*)src + 8) - 1);

    d = (*(struct S***)(self + 0x38))[idx];
    check = *(int*)(*(char**)(self + 0x1c) + 0x18);
    if (check >= 0) {
        func_ov007_020c281c((char*)d, check);
        func_ov007_020c27b4(d);
        func_ov007_020c26fc((char*)d);
    }

    func_ov007_020c6e68(self, idx);
}
