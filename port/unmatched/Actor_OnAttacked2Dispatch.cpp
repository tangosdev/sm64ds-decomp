/* HOST COPY of src/func_ov002_020ef070.cpp -- the slot-23 (Actor::OnAttacked2)
 * dispatcher.
 *
 * WHY THIS FILE EXISTS. Once its RaycastLine finds an actor by collision id,
 * this function dispatches that actor's vtable slot 23 (OnAttacked2). The
 * byte-locked src models the vtable as a plain array of function pointers
 * taking self explicitly:
 *
 *     (*(void (**)(void *, char *))(*(int *)a + 0x5c))(a, actor)
 *
 * which MSVC compiles __cdecl: BOTH arguments are pushed, the receiver is NOT
 * left in ECX as `this`, and the CALLER cleans the stack (a batched
 * `add esp,10h`). Every seated slot-23 body in this port is __thiscall: the
 * receiver arrives in ECX and the CALLEE pops the one pushed argument. One
 * vtable word cannot satisfy both. Until this site is thiscall, slot 23 cannot
 * hold a real body -- ac_atk2 and every override emits `ret 4` against a
 * caller that also cleans up, over-popping by one seat on every dispatch.
 *
 * This is the same divergence, and the same fix, that slot 28 has in
 * unmatched/Player_HeadBonk.cpp. This copy is the matched source above, line
 * for line, with exactly ONE thing changed: the dispatch goes through a C++
 * virtual so MSVC emits __thiscall (receiver in ECX, one push, no caller
 * cleanup). Every constant, every field offset, every branch is the
 * byte-locked original's. src/ is untouched; slice_gate10.txt carries this
 * file in place of the .cpp, the Player_HeadBonk pattern.
 *
 * WHAT IT IS IN THE GAME. A collision cast picks a target and tells it it was
 * attacked (slot 23, scale 0x64 -- the narrower of the two OnAttacked casts).
 */
typedef struct { int x, y, z; } Vector3;
typedef struct {
    void *tag;
    int f04, f08, f0c, f10, f14;
    unsigned short f18, f1a;
    int f1c, f20, f24;
} ClsnResultTmp;

extern "C" {
extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *act);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void func_ov002_020d8838(void *actor);
extern unsigned _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(unsigned id);
extern void _ZN10ClsnResultD1Ev(void *self);

extern int data_02099368;
extern short data_02082214[];
}

/* The one changed line's shape. Twenty-three filler virtuals put the
   twenty-fourth at vtable+0x5c, which is slot 23 (0x5c/4), so the call MSVC
   emits is `call dword ptr [reg+0x5c]` with the argument pushed and the callee
   popping it -- byte for byte what a thiscall slot-23 site emits. Declared and
   never defined: only the call is wanted, and the object is always a real
   actor carrying a real vtable. */
struct Atk2Actor {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22();
    virtual void OnAttacked2(char *other);   /* vtable + 0x5c */
};

extern "C" int func_ov002_020ef070(void *unused, char *actor)
{
    Vector3 v1, v2;
    ClsnResultTmp tmp;
    char rl[0x78];

    _ZN11RaycastLineC1Ev(rl);

    Vector3 *pos = (Vector3 *)(((long long)(int)(actor + 0x5c)));
    int x = pos->x;
    v1.x = x;
    int y = pos->y;
    v1.y = y;
    int z = pos->z;
    v2.x = x;
    v2.z = z;
    v1.z = z;
    v1.y = y + 0x3c000;
    v2.y = y + 0x3c000;

    int scale = 0x64;
    if (*(int *)(actor + 8) == 2) scale = 0x82;
    v2.x = scale * data_02082214[(*(unsigned short *)(actor + 0x8e) >> 4) << 1] + v2.x;
    v2.z = scale * data_02082214[((*(unsigned short *)(actor + 0x8e) >> 4) << 1) + 1] + v2.z;

    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &v1, &v2, actor);
    if (_ZN11RaycastLine10DetectClsnEv(rl)) {
        int t = (*(unsigned short *)(actor + 0xc) == 0xbf);
        if (t != false) {
            func_ov002_020d8838(actor);
        }
        {
            int *dst = &tmp.f04;
            int w0 = *(int *)(rl + 0x14);
            int w1 = *(int *)(rl + 0x18);
            dst[0] = w1 ? w0 : w0;
            dst[1] = w1;
            dst[2] = *(int *)(rl + 0x1c);
            dst[3] = *(int *)(rl + 0x20);
            dst[4] = *(int *)(rl + 0x24);
            tmp.tag = &data_02099368;
            tmp.f18 = *(unsigned short *)(rl + 0x28);
            tmp.f1a = *(unsigned short *)(rl + 0x2a);
            tmp.f1c = *(int *)(rl + 0x2c);
            tmp.f20 = *(int *)(rl + 0x30);
            tmp.f24 = *(int *)(rl + 0x34);
            if (_ZNK10ClsnResult9GetClsnIDEv(&tmp) != 0xffffffff) {
                void *a = _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(&tmp));
                if (a) {
                    ((Atk2Actor *)a)->OnAttacked2(actor);
                    _ZN10ClsnResultD1Ev(&tmp);
                    _ZN11RaycastLineD1Ev(rl);
                    return 1;
                }
            }
            _ZN10ClsnResultD1Ev(&tmp);
        }
    }
    _ZN11RaycastLineD1Ev(rl);
    return 0;
}
