//cpp
// @symbol _ZN6Player19St_SwingPlayer_MainEv
/* recovered: Player::St_SwingPlayer_Main, the per-frame body of the swing state (animation, particles, the approach helpers). */
// NONMATCHING: div 8 of 241 words. mwccarm 2004/b56, --module ov002,
// @ 0x020d9fec size 0x3c4. Residue class: build delta (colouring).
// Draft from nearmiss/db.jsonl (stored divergence 8), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

struct Vec3 { int x, y, z; };


extern "C" {
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, u32 anim, int a, s32 b, u16 d);
extern void func_ov002_020d9c70(char* c);
extern int func_ov002_020da95c(char* c);
extern int func_ov002_020da9d4(char* c);
extern int _Z15ApproachLinear2Rsss(s16* ref, s16 target, s16 step);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 h, u32 id, s32 x, s32 y, s32 z, void* v, void* cb);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 h, u32 a, u32 b, void* v, u32 d);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void* v);
extern void func_ov002_020dc174(char* c, void* p, int a, int b, u32 e, u32 f);
extern void _ZN5dCc_c5ClearEv(char* c);
extern void _ZN5dCc_c6UpdateEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern void Player_AdvanceAnims(char* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern int data_ov002_0211013c[];
}

extern "C" int _ZN6Player19St_SwingPlayer_MainEv(char* c)
{
    switch (*(u8*)(c + 0x6e3)) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6d, 0x40000000, 0x1000, 0);
            *(u8*)(c + 0x6e3) = 1;
        }
        *(s16*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFFLL) += *(s16*)(c + 0x69c);
        break;
    case 1: {
        int arr[3];
        Vec3 v;
        if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 1) != 0) {
            char* p = *(char**)(c + 0x358);
            if (p != 0) {
                int b = (*(u32*)(p + 0xb0) & 0x200) != 0;
                if (!b) {
                    func_ov002_020d9c70(c);
                    func_ov002_020da95c(c);
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6e, 0x40000000, 0x1000, 0);
                    *(u8*)(c + 0x6e3) = 3;
                } else {
                    func_ov002_020da9d4(c);
                }
            }
            return 1;
        }
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
            if (step != 0) {
                *(s16*)(c + 0x69c) += step;
                if (*(s16*)(c + 0x69c) >= 0x1800)
                    *(s16*)(c + 0x69c) = 0x1800;
                else if (*(s16*)(c + 0x69c) <= -0x1800)
                    *(s16*)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);
        }
        {
            s16 a2 = *(s16*)(c + 0x69c);
            if (a2 < 0) a2 = -a2;
            if (a2 >= 0xf99) {
                u32 id = 0x132;
                v.x = *(int*)(c + 0x5c);
                v.y = *(int*)(c + 0x60);
                v.z = *(int*)(c + 0x64);
                v.y = *(int*)(c + 0x60) + 0x3c000;
                if (*(s16*)(c + 0x69c) > 0)
                    id = 0x133;
                *(u32*)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(u32*)(c + 0x628), id, *(int*)&v.x, *(int*)&v.y, *(int*)&v.z, 0, 0);
                *(u32*)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(u32*)(c + 0x620), 0, 0x1c, c + 0x74, 0);
            }
            if (*(s16*)(c + 0x69c) != 0)
                *(u16*)(c + 0x6a4) = 0xa;
        }
        if (*(u16*)(c + 0x6a4) == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6f, 0x40000000, 0x1000, 0);
            *(u8*)(c + 0x6e3) = 2;
        }
        {
            s16 before = *(s16*)(c + 0x8e);
            *(s16*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFFLL) += *(s16*)(c + 0x69c);
            if ((*(s16*)(c + 0x69c) <= -0x100 && before < *(s16*)(c + 0x8e)) ||
                (*(s16*)(c + 0x69c) >= 0x100 && before > *(s16*)(c + 0x8e)))
                _ZN5Sound9PlayBank0EjRK7Vector3(0x1b, c + 0x74);
        }
        arr[0] = 0;
        arr[1] = 0x32000;
        arr[2] = 0x64000;
        func_ov002_020dc174(c, arr, 0x32000, 0x32000, 0x80, 0);
        _ZN5dCc_c5ClearEv(c + 0x314);
        _ZN5dCc_c6UpdateEv(c + 0x314);
        break;
    }
    case 2:
        if (_ZN6Player12FinishedAnimEv(c))
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        break;
    case 3:
        if (_ZN6Player12FinishedAnimEv(c))
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        break;
    }

    Player_AdvanceAnims(c);
    return 1;
}
