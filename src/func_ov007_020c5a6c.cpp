//cpp
typedef unsigned short u16;
struct Vec3 { int v[3]; };

extern "C" {
    int func_ov007_020c30ac(void* arr, int count, void* target, int arg4, int* out);
    void func_ov007_020c64c4(char* c, int b);
    void SubVec3(struct Vec3* a, struct Vec3* b, struct Vec3* c);
    int LenVec3(struct Vec3* v);
}

extern "C" void func_ov007_020c5a6c(char* self, void* target, int flag)
{
    *(int*)(self + 0x5c) = -1;
    *(int*)(self + 0x58) = *(int*)(self + 0x5c);

    int i;
    int carry = -1;
    int out;
    for (i = 0; i < *(int*)(self + 8); i++) {
        void** arr38 = *(void***)(self + 0x38);
        void** arr28 = *(void***)(self + 0x28);
        u16 cnt = *(u16*)((char*)arr38[i] + 8);
        int best = func_ov007_020c30ac(arr28[i], cnt, target, carry, &out);
        if (best != -1) {
            if (flag == 0)
                goto upd;
            if (flag == 0)
                goto skip;
            if (i < *(int*)(self + 0xac))
                goto upd;
            if (i != *(int*)(self + 0xac))
                goto skip;
            if (best >= *(int*)(self + 0xb0))
                goto skip;
upd:
            *(int*)(self + 0x58) = i;
            *(int*)(self + 0x5c) = best;
            carry = out;
skip:;
        }
    }

    int bi = *(int*)(self + 0x58);
    if (bi == -1)
        return;
    int bj = *(int*)(self + 0x5c);
    if (bj == -1)
        return;

    char* p1 = *(char**)(self + 0x3c);
    char* p2 = *(char**)(p1 + bi * 4);
    char* p3 = *(char**)p2;
    Vec3** elem = *(Vec3***)(p3 + bj * 4);

    switch (*(int*)(self + 4)) {
    case 0:
        *(Vec3*)(self + 0x70) = **elem;
        func_ov007_020c64c4(self, 1);
        break;
    case 1:
        SubVec3((Vec3*)(self + 0x70), *elem, (Vec3*)(self + 0x60));
        break;
    case 2:
        {
            *(int*)(self + 0x78) = *(int*)(self + 0xa0);
            *(Vec3*)(self + 0x98) = **elem;
            char* r4 = *(char**)(self + 0xa4);
            SubVec3((Vec3*)(*(int**)r4)[0], (Vec3*)(*(int**)(r4 + 4))[0], (Vec3*)(r4 + 0xc));
            *(int*)(r4 + 8) = LenVec3((Vec3*)(r4 + 0xc));
        }
        break;
    }
}
