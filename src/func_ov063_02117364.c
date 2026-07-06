typedef signed int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef signed char s8;
typedef int Fix12i;

struct ParticleCallback;

typedef struct { s32 x, y, z; } Vec3;

typedef struct {
    u8 pad0[0xbe];
    u16 fbe;
    u16 fc0;
    u16 fc2;
    u16 fc4;
    u8 pad1[0xe];
    u16 lo8 : 8;
    u16 flag : 1;
    u16 hi7 : 7;
} Sub;

typedef struct {
    u16 lo8 : 8;
    u16 flag : 1;
    u16 hi7 : 7;
} FlagW;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 unk, Fix12i x, Fix12i y, Fix12i z, void* rot, struct ParticleCallback* cb);
extern void func_ov063_02117650(void* c);
extern void func_02012694(int a, void* b, void* d, int e);
extern int RandomIntInternal(int* seed);
extern int data_0209e650;

#define New _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE

void func_ov063_02117364(void* c)
{
    volatile Vec3 pos;
    pos.x = *(s32*)((char*)c + 0x5c);
    pos.y = *(s32*)((char*)c + 0x60);
    pos.z = *(s32*)((char*)c + 0x64);

    if (((Sub*)((char*)c + 0x500))->fc2 != 0)
        (*(u16*)(((int)c + 0x5c2) & 0xFFFFFFFFFFFFFFFFLL))--;

    if (*(u8*)((char*)c + 0x5c9) == 0) {
        if (*(u8*)((char*)c + 0x5c8) == 0) {
            if (((Sub*)((char*)c + 0x500))->fc2 != 0)
                return;
            {
                FlagW* fw = (FlagW*)(((int)c + 0x5d4) & 0xFFFFFFFFFFFFFFFFLL);
                fw->flag ^= 1;
            }
            func_ov063_02117650(c);
            *(u8*)((char*)c + 0x5c9) = 0xff;
            *(u32*)((char*)c + 0x5dc) = 0;
            *(u32*)((char*)c + 0x5d8) = *(u32*)((char*)c + 0x5dc);
            ((Sub*)((char*)c + 0x500))->fc2 = 0x78;
            func_02012694(0x154, (char*)c + 0x74, (char*)c + 0x500, 0x78);
            return;
        }
        pos.y += *(s32*)((char*)c + 0x584) * 0xaf;
        if (((Sub*)((char*)c + 0x500))->flag)
            pos.x = pos.x * -1;
        *(u32*)((char*)c + 0x5d8) = New(*(volatile u32*)((char*)c + 0x5d8), 0x95, pos.x, pos.y, pos.z, 0, 0);
        *(u32*)((char*)c + 0x5dc) = New(*(volatile u32*)((char*)c + 0x5dc), 0x96, pos.x, pos.y, pos.z, 0, 0);
        return;
    }

    if (((Sub*)((char*)c + 0x500))->fc2 > 0x1c) {
        pos.y += *(s32*)((char*)c + 0x584) * 0xaf;
        if (((Sub*)((char*)c + 0x500))->flag)
            pos.x = pos.x * -1;
        *(u32*)((char*)c + 0x5d8) = New(*(volatile u32*)((char*)c + 0x5d8), 0x97, pos.x, pos.y, pos.z, 0, 0);
        *(u32*)((char*)c + 0x5dc) = New(*(volatile u32*)((char*)c + 0x5dc), 0x98, pos.x, pos.y, pos.z, 0, 0);
    }
    if (*(u8*)((char*)c + 0x5c8) != 0xff)
        return;
    if (((Sub*)((char*)c + 0x500))->fc2 > 0x1c)
        ((Sub*)((char*)c + 0x500))->fc2 = 0x1c;
    if (((Sub*)((char*)c + 0x500))->fc2 != 0)
        return;
    *(u8*)((char*)c + 0x5cc) = 1;
    *(u32*)(((int)c + 0x19c) & 0xFFFFFFFFFFFFFFFFLL) &= ~1u;
    ((Sub*)((char*)c + 0x500))->fbe = ((u32)RandomIntInternal(&data_0209e650) >> 16 & 0x3f) + 0x3c;
    ((Sub*)((char*)c + 0x500))->fc4 = ((u32)RandomIntInternal(&data_0209e650) >> 16) % 150 + 0x12c;
    *(u32*)((char*)c + 0x5dc) = 0;
    *(u32*)((char*)c + 0x5d8) = *(u32*)((char*)c + 0x5dc);
}
