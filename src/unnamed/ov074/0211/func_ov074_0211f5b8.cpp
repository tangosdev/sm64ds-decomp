//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))

extern void* data_0209f318;

extern "C" {
extern int _ZN9Animation8FinishedEv(void* c);
extern void func_ov074_02121a4c(char* c, int idx);
extern void func_ov074_0211f154(char* c);
extern int _ZN6Player12GetTalkStateEv(void* p);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void* p, void* actor, u32 id, const Vector3* v, u32 a, u32 b);
extern void func_0201277c(u32 a);
extern u16 DecIfAbove0_Short(u16* p);
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void* prev);
extern void _ZN8dActor_c10PoofDustAtERK7Vector3(void* c, const Vector3* v);
extern void _ZN7fBase_c18MarkForDestructionEv(void* a);
extern void func_02012694(u32 a, void* b);
extern int func_ov074_0211f38c(char* c);
extern int func_ov074_02120474(char* c);
extern int func_ov074_021204c0(char* c);
extern int _ZN6Player18HasFinishedTalkingEv(void* p);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12 x, Fix12 y, Fix12 z);
extern void func_ov074_0211f344(void* r6, u8 r5);
extern void func_ov074_0211f244(char* c);
extern void _ZN7Message7EndTalkEv();
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev();
extern void func_02011cfc();
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, Fix12 f);
extern void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 kind, const Vector3* v, void* v16, s32 a, s32 b);
}

extern "C" void func_ov074_0211f5b8(char* c)
{
    Vector3 vzero;
    Vector3 vmsg;
    void* player;
    s32 state;
    void* data0209f318;

    vzero.x = 0;
    vzero.y = 0;
    vzero.z = 0;
    vmsg.x = *(s32*)(c + 0x5c);
    vmsg.y = *(s32*)(c + 0x60);
    vmsg.z = *(s32*)(c + 0x64);
    vmsg.y = vmsg.y + 0x78000;
    data0209f318 = data_0209f318;
    player = *(void**)(c + 0x5c8);

    if (_ZN9Animation8FinishedEv(c + 0x260) != 0) {
        func_ov074_02121a4c(c, 4);
    }

    switch (*(u8*)(c + 0x603)) {
    case 0:
        func_ov074_0211f154(c);
        {
            u16 msgId = (u16)(*(s32*)((char*)player + 8) + 0xd7);
            state = _ZN6Player12GetTalkStateEv(player);
            if (state != 0) {
                if (state == 1)
                    return;
                if (state == 2)
                    goto inc603;
                return;
            }
            if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, c, (s16)msgId, &vmsg, 1, 2) == 0)
                return;
            func_0201277c(0x15a);
        }
        return;
    inc603:
        *(u8*)AT(c, 0x603) += 1;
        return;
    case 1:
        if (DecIfAbove0_Short((u16*)(c + 0x5fc)) == 0) {
            void* actor = _ZN8dActor_c15FindWithActorIDEjPS_(0xc8, 0);
            if (actor != 0) {
                u32 id = 0xc8;
                while (1) {
                    s32 flag = (*(u32*)((char*)actor + 0xb0) & 0x40000) ? 1 : 0;
                    if (flag == 0) {
                        Vector3 v2;
                        s32* src = (s32*)AT(actor, 0x5c);
                        v2.x = src[0];
                        v2.y = src[1];
                        v2.z = src[2];
                        _ZN8dActor_c10PoofDustAtERK7Vector3(c, &v2);
                        _ZN7fBase_c18MarkForDestructionEv(actor);
                        *(u16*)(c + 0x5fc) = 2;
                        return;
                    }
                    actor = _ZN8dActor_c15FindWithActorIDEjPS_(id, actor);
                    if (actor == 0)
                        break;
                }
            }
        }
        if (*(u8*)(c + 0x602) == 1) {
            void* a2 = _ZN8dActor_c15FindWithActorIDEjPS_(0xc8, 0);
            if (a2 != 0) {
                s32 flag2;
                if (*(u32*)((char*)a2 + 0xb0) & 0x40000)
                    flag2 = 1;
                else
                    flag2 = 0;
                if (flag2 != 0) {
                    *(u8*)AT(c, 0x602) -= 1;
                }
            }
        }
        if (*(u8*)(c + 0x602) != 0)
            return;
        func_02012694(0x15c, c + 0x74);
        *(u8*)AT(c, 0x603) += 1;
        *(u8*)(c + 0x602) = 0;
        return;
    case 2:
        if (*(u8*)(c + 0x60b) == 0 && func_ov074_0211f38c(c) != 0) {
            *(u8*)(c + 0x60b) = 1;
        }
        {
            s32 r4 = func_ov074_02120474(c);
            if (func_ov074_021204c0(c) == 0)
                return;
            if (r4 == 0)
                return;
        }
        *(u8*)(c + 0x602) = 0;
        *(u8*)AT(c, 0x603) += 1;
        return;
    case 3:
        if (_ZN6Player18HasFinishedTalkingEv(player) == 0)
            return;
        *(u8*)(c + 0x60a) = 0;
        vzero.x = *(s32*)(c + 0x5c);
        vzero.y = *(s32*)(c + 0x60);
        vzero.z = *(s32*)(c + 0x64);
        vzero.y = vzero.y + 0x190000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x91, vzero.x, vzero.y, vzero.z);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x92, vzero.x, vzero.y, vzero.z);
        func_02012694(0x15f, c + 0x74);
        func_ov074_0211f344(c, 1);
        func_ov074_0211f244(c);
        *(s32*)AT(data0209f318, 0x154) &= ~8;
        _ZN7Message7EndTalkEv();
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
        *(u8*)(c + 0x608) = 1;
        *(u16*)(c + 0x500 + 0xfc) = 0xf;
        *(u8*)AT(c, 0x603) += 1;
        return;
    case 4:
        vzero.x = *(s32*)(c + 0x5c);
        vzero.y = *(s32*)(c + 0x60);
        vzero.z = *(s32*)(c + 0x64);
        vzero.y = vzero.y + 0x12c000;
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x11a, 2, &vzero, 0, *(signed char*)(c + 0xcc), -1);
        func_02012694(0xbb, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
        return;
    default:
        return;
    }
}
