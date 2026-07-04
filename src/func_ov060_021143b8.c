extern int func_ov060_0211469c(void* c);
extern void func_02012694(int a, void* b);
extern void func_ov060_021145a8(void* c);
extern void func_ov060_02115018(void* c);
extern int func_ov060_021145d4(void* c);
extern void func_0200fa04(void* c, void* v, int a);
extern void func_ov060_02111cc0(char* c, int idx, int b);
extern void func_ov060_02115b0c(char* c);
extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* p);
extern int func_ov060_02115a30(void* o);

void func_ov060_021143b8(char* self)
{
    unsigned char st = *(unsigned char*)(self + 0x423);
    int vec[3];

    if (st == 0) {
        if (func_ov060_0211469c(self) == 0) return;
        if (*(unsigned char*)(self + 0x414) == 2 && (*(int*)(self + 0x418) & 0x10000)) {
            *(int*)(self + 0xa8) = 0x28000;
        } else {
            *(int*)(self + 0xa8) = 0x32000;
        }
        func_02012694(0xb1, self + 0x74);
        *(short*)(self + 0x3fe) = 0;
        *(int*)(self + 0x98) = 0;
        func_ov060_021145a8(self);
        if (*(int*)(self + 0x98) == 0) {
            *(int*)(self + 0x5c) = *(int*)(self + 0x3c8);
            *(int*)(self + 0x64) = *(int*)(self + 0x3d0);
        }
        (*(unsigned char*)(((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
        *(int*)(self + 0x3f8) = 0x1000;
        return;
    }
    if (st == 1) {
        if (*(unsigned char*)(self + 0x414) == 2 && (*(int*)(self + 0x418) & 0x10000)) {
            func_ov060_02115018(self);
        }
        if (func_ov060_021145d4(self) == 0) {
            if (*(int*)(self + 0x60) >= *(int*)(self + 0x3cc)) return;
        }
        if (*(int*)(self + 0xa8) != 0) {
            vec[0] = *(int*)(self + 0x5c);
            vec[1] = *(int*)(self + 0x60);
            vec[2] = *(int*)(self + 0x64);
            func_0200fa04(self, vec, 0);
            func_ov060_02111cc0(self, 0xc, 0x40000000);
        }
        *(int*)(((int)self + 0x418) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x10000;
        *(int*)(self + 0x98) = 0;
        *(int*)(self + 0xa8) = 0;
        *(int*)(self + 0x60) = *(int*)(self + 0x3cc);
        (*(unsigned char*)(((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
        func_ov060_02115b0c(self);
        if (*(unsigned char*)(self + 0x414) == 1) {
            void* a;
            *(int*)(self + 0x40c) = 0x13;
            a = _ZN5Actor15FindWithActorIDEjPS_(0xa6, 0);
            if (a != 0) {
                *(int*)(self + 0x3ac) = *(int*)((char*)a + 4);
            }
        }
        return;
    }
    if (func_ov060_02115a30(self) != 0) {
        *(int*)(self + 0x40c) = 0;
    }
}
