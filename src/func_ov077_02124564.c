struct Vec3 { int x, y, z; };
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
void* _ZN5Actor22ClosestNonVanishPlayerEv(void* p);
int Vec3_HorzDist(const struct Vec3* a, const struct Vec3* b);
int func_ov077_02123880(void* c);
void func_ov077_0212478c(void* c, int v);
void func_ov077_0212390c(char* c);
void func_ov077_02123814(char* c);
void _ZN9Animation7AdvanceEv(void* p);
void func_ov077_02123a74(void* thiz);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* thiz, const struct Vec3* v);
void _ZN12CylinderClsn5ClearEv(void* p);
void _ZN12CylinderClsn6UpdateEv(void* p);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const struct Vec3* pos, unsigned int e);

extern int data_ov077_02127b88[3];

int func_ov077_02124564(char* c) {
    *(int*)(c + 0x98) = 0xc000;
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x41c)) == 0) {
        char* p = (char*)_ZN5Actor22ClosestNonVanishPlayerEv(c);
        if (p == 0) goto store2;
        if (*(int*)(p + 0x60) >= *(int*)(c + 0x60)) goto store1;
        if (Vec3_HorzDist((struct Vec3*)(c + 0x5c), (struct Vec3*)(p + 0x5c)) >= 0x190000) goto store1;
        if (func_ov077_02123880(c) >= 4) goto store1;
        func_ov077_0212478c(c, 1);
        goto tail;
    store1:
        *(unsigned char*)(c + 0x41c) = 0x96;
        goto tail;
    store2:
        *(unsigned char*)(c + 0x41c) = 0x96;
    }
tail:
    func_ov077_0212390c(c);
    func_ov077_02123814(c);
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN9Animation7AdvanceEv(c + 0x1b0);
    func_ov077_02123a74(c);
    {
        struct Vec3 pos;
        pos.y = data_ov077_02127b88[1] + *(int*)(c + 0x414);
        pos.x = data_ov077_02127b88[0];
        pos.z = data_ov077_02127b88[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1c4, &pos);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x1c4);
    _ZN12CylinderClsn6UpdateEv(c + 0x1c4);
    *(int*)(c + 0x410) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(int*)(c + 0x410), 3, 0x182, (struct Vec3*)(c + 0x74), 0);
    return 1;
}
