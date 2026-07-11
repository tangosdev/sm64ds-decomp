//cpp
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

class Actor;
class Player;

extern "C" Actor *_ZN5Actor10FindWithIDEj(u32 id);
extern "C" void func_ov060_021123a0(void *a, int b);
extern "C" int _ZN6Player15IsCollectingCapEv(Player *p);
extern "C" int _ZN6Player7TryGrabER5Actor(Player *p, Actor &a);

extern "C" void func_ov060_02115d68(char *self) {
    u32 *p = (u32 *)((char *)(((int)self + 0xec) & 0xFFFFFFFFFFFFFFFF));
    Actor *r5 = _ZN5Actor10FindWithIDEj(*(u32 *)(self + 0x108));
    *p &= ~1u;
    if (*(s32 *)((char *)r5 + 0x40c) != 0x13) goto skip_13;
    func_ov060_021123a0(r5, 1);
    return;
skip_13:
    if (*(s32 *)((char *)r5 + 0x40c) == 4) return;
    func_ov060_021123a0(r5, 1);
    if (*(u32 *)(self + 0xf8) == 0) return;
    if (!(*(u32 *)(self + 0xf4) & 0x1000)) return;
    Actor *r4 = _ZN5Actor10FindWithIDEj(*(u32 *)(self + 0xf8));
    if (!r4) return;
    if (_ZN6Player15IsCollectingCapEv((Player *)r4)) return;
    if (!_ZN6Player7TryGrabER5Actor((Player *)r4, *(Actor *)self)) return;
    s16 *ip = (s16 *)((char *)r5 + 0x8c);
    *(s16 *)((char *)r4 + 0x8c) = *(s16 *)((int)ip & 0xFFFFFFFFFFFFFFFF);
    *(s16 *)((char *)r4 + 0x8e) = ip[1];
    *(s16 *)((char *)r4 + 0x90) = ip[2];
    *(s16 *)((char *)r4 + 0x92) = *(s16 *)((int)ip & 0xFFFFFFFFFFFFFFFF);
    *(s16 *)((char *)r4 + 0x94) = ip[1];
    *(s16 *)((char *)r4 + 0x96) = ip[2];
    *(s32 *)(self + 0x110) = 2;
    *(s32 *)(self + 0x10c) = (s32)r4;
    func_ov060_021123a0(r5, 0);
    *(s32 *)((char *)r5 + 0x410) = 1;
    *(s32 *)((char *)r5 + 0x3a4) = (s32)r4;
    *(s16 *)(self + 0x116) = 0x96;
}
