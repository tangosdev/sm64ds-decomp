//cpp
typedef short s16;
typedef unsigned short u16;
struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };

struct C;
typedef void (C::*PMF)();

extern "C" u16 DecIfAbove0_Short(u16* p);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
extern "C" int func_ov016_02111c40(void* self);
extern "C" void* _ZN5Actor10FindWithIDEj(unsigned id);
extern "C" void* _ZN5Actor13ClosestPlayerEv(void* self);
extern "C" int Vec3_Dist(const void* a, const void* b);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern "C" void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned a, unsigned b, const Vector3& pos, const Vector3_16* rot, int e, int f);
extern "C" void func_ov016_02111284(void* self);
extern "C" void _ZN12CylinderClsn5ClearEv(void* self);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* self);
extern "C" void _ZN14BlendModelAnim7AdvanceEv(void* self);

struct Disp { int unused[2]; PMF pmf; };

extern "C" unsigned char data_0209f220;
extern void* data_ov016_02114dbc;

#define LA(p) ((void*)(unsigned long)(((long long)(int)(unsigned long)(p)) & 0xFFFFFFFFFFFFFFFFLL))

extern "C" int _ZN5Unagi8BehaviorEv(char* c) {
    void* found;
    unsigned id;

    DecIfAbove0_Short((u16*)(c + 0x100));
    {
        Disp* d = *(Disp**)(c + 0x34c);
        if (d->pmf) {
            (((C*)c)->*(d->pmf))();
        }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
    func_ov016_02111c40(c);

    id = *(unsigned*)(c + 0x49c);
    if (id != 0) {
        found = _ZN5Actor10FindWithIDEj(id);
        if (found == 0)
            goto clear_id;
        if (*(int*)(c + 0x408) == 1) {
            void* cp = _ZN5Actor13ClosestPlayerEv(c);
            if (cp != 0) {
                int off43c = 0x43c;
                if (Vec3_Dist(c + off43c, (char*)cp + 0x5c) < 0xfa000) {
                    _ZN9ActorBase18MarkForDestructionEv(found);
                    {
                        int param = *(signed char*)(c + 0xcc);
                        int m1 = -1;
                        int off = 0x43c;
                        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                            0xb2, *(unsigned char*)(c + 0x414) | 0x40,
                            *(Vector3*)(c + off), (Vector3_16*)0,
                            param, m1);
                    }
                    *(int*)(c + 0x49c) = 0;
                    goto after_first;
                }
            }
        }
        goto after_first;
    clear_id:
        *(int*)(c + 0x49c) = 0;
    after_first:
        if (*(int*)(c + 0x49c) != 0) {

        int x = *(int*)(c + 0x43c);
        int off = 0x416;
        *(int*)((char*)found + 0x5c) = x;
        short* r = (short*)LA(c + off);
        *(int*)((char*)found + 0x60) = *(int*)(c + 0x440);
        *(int*)((char*)found + 0x64) = *(int*)(c + 0x444);
        *r += 0x1000;

        }
    }

    func_ov016_02111284(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    if (data_0209f220 == 1 && *(void**)(c + 0x34c) != &data_ov016_02114dbc) {
        _ZN12CylinderClsn5ClearEv(c + 0x150);
        _ZN12CylinderClsn6UpdateEv(c + 0x150);
    }
    _ZN14BlendModelAnim7AdvanceEv(c + 0x350);
    return 1;
}
