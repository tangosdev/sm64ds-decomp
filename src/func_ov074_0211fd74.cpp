//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vec3 { s32 x, y, z; };

extern "C" {
    int func_ov074_02121a20(void* c, int idx);
    int func_ov074_021207b8(void* c);
    void func_ov074_02121a4c(void* c, int i);
    int func_ov074_021206c8(void* c);
    void* _ZN5Actor13ClosestPlayerEv(void* self);
    int _ZN9Animation8FinishedEv(void* anim);
    u16 DecIfAbove0_Short(u16* p);
    void func_ov074_0212087c(void* out, void* self, int idx);
    void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void* self, void* v);
    void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, void* pos, void* rot, int e, int f);
    void func_ov084_021296b0(void* a, void* b);
    void func_ov074_021203e4(void* c, int i);
    int __aeabi_idiv(int a, int b);
}

extern u8 data_ov074_02122d80[];

extern "C" void func_ov074_0211fd74(void* self)
{
    u8* c = (u8*)self;

    if (func_ov074_02121a20(self, 6) != 0) {
        if (func_ov074_021207b8(self) == 0) return;
        func_ov074_02121a4c(self, 0xb);
        return;
    } else {
        if (func_ov074_02121a20(self, 0xb) != 0) goto L90;
        if (func_ov074_021206c8(self) == 0) return;
        func_ov074_02121a4c(self, 0xb);
        return;
    }

L90:;
    int r5 = data_ov074_02122d80[*(u8*)(c+0x604)];
    u8* pl = (u8*)_ZN5Actor13ClosestPlayerEv(self);
    if (*(u8*)(c+0x604) == 1 && *(s32*)(pl+8) != 3) r5 = r5 + 1;

    if (_ZN9Animation8FinishedEv((void*)(c+0x260)) == 0) return;
    if (DecIfAbove0_Short((u16*)(c+0x5fc)) != 0) return;

    Vec3 pos;
    func_ov074_0212087c(&pos, self, *(u8*)(c+0x602));
    Vec3 pp;
    pp.x = pos.x;
    pp.y = pos.y;
    pp.z = pos.z;
    _ZN5Actor19DisappearPoofDustAtERK7Vector3(self, &pp);

    u32 sid = 0xeeee;
    if (*(s32*)(pl+8) != 3) {
        if (*(u8*)(c+0x602) % 2 == 0) sid = 0xeeef;
    }

    u8* sp2 = (u8*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xc8, sid, &pos, (void*)(c+0x8c), *(signed char*)(c+0xcc), -1);
    *(s32*)(sp2+0x43c) = *(s32*)(c+4);
    Vec3 v2;
    v2.x = *(s32*)(c+0x5d0);
    v2.y = *(s32*)(c+0x5d4);
    v2.z = *(s32*)(c+0x5d8);
    func_ov084_021296b0(sp2, &v2);

    *(u8*)(sp2+0x474) = *(u8*)(c+0x602);
    u8* p602 = (u8*)(((int)c + 0x602) & 0xFFFFFFFFFFFFFFFF);
    *p602 = (u8)(*p602 + 1);
    *(u16*)((c+0x500)+0xfc) = 2;

    u8* pl2 = (u8*)_ZN5Actor13ClosestPlayerEv(self);
    int dv = 3;
    if (*(s32*)(pl2+8) != 3) {
        if (*(u8*)(c+0x604) == 1) dv = 5;
    }
    if (*(u8*)(c+0x602) % dv == 0) *(s32*)(c+0x268) = 0;

    if (*(u8*)(c+0x602) < r5) return;
    func_ov074_021203e4(self, 3);
}
