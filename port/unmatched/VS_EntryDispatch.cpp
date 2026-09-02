/* dScEntry_c's THREE POINTER-TO-MEMBER DISPATCH SITES, host copies -- the
 * MgTrampolineTerror_StateDispatch treatment, applied to the VS menu.
 *
 * THE DEFECT CLASS (measured, not predicted): the scene's states live as
 * {fn, delta} pairs at self+0x54/+0x5c/+0x64, seeded by func_ov075_0211a194
 * from six-word records the sinits build in bss, and dispatched by Behavior
 * (func_ov075_0211a2b8), Render (func_ov075_0211a26c) and the setter itself
 * through `(self->**pmf)()`. The matched TUs model that with a real MSVC
 * pointer-to-member, which compiles to a __thiscall: `this` rides in ecx and
 * nothing goes to the stack. The fn words hold plain cdecl host bodies (the
 * seat's generated patch put them there), so the receiver never arrives --
 * the first VS lobby boot faulted in state func_ov075_02119b34 at +0x37
 * reading [esi+0x50] with esi = stack garbage (1). Same story for Behavior's
 * element walk, which the TU models as a virtual call `o->v0()` over the
 * 0x24-stride records at self+0x70 whose vptr is the mounted
 * data_ov075_0211c94c table.
 *
 * THE COPIES CHANGE THE CALL SHAPE AND NOTHING ELSE: every dispatch becomes
 * an explicit cdecl call through the record word with self as the first
 * argument -- which is exactly what the mwcc pair-dispatch does on ARM
 * (r0 = self + delta, delta 0 throughout this single-inheritance scene).
 * The matched TUs stay in src/, byte-verified, excluded from the slice; the
 * fader dispatch data_0209f5bc->v5() keeps the TU's own shape because that
 * object's host vtable already serves every other scene's matched TUs.
 *
 * src/func_ov075_0211a194.cpp, src/func_ov075_0211a2b8.cpp and
 * src/func_ov075_0211a26c.cpp are out of the slice for these.
 */

#include <cstdio>

extern "C" {
void func_020551f0(void *s, int v);
int  func_ov075_02119dc4(void *c, void *arg);
int  func_0203083c(void);
extern int data_0209fc68;
extern int data_ov075_0211d930[];
}

/* the fader object: the TU's own model, unchanged -- its host vtable serves
   every scene's matched dispatches already */
struct VsVObj {
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4(); virtual int v5();
};
extern "C" { extern VsVObj *data_0209f5bc; }

typedef void (*VsStateFn)(void *self);

static void vs_state_call(void *self, unsigned off)
{
    char *c = (char *)self;
    VsStateFn fn = *(VsStateFn *)(c + off);
    /* the mwcc pair: fn word at +off, delta word at +off+4; the ROM calls
       fn(self + delta) and every record this scene builds carries delta 0 */
    char *recv = c + *(int *)(c + off + 4);
    if (fn)
        fn(recv);
}

// PORT_HOST_ABI: mwcc pointer-to-member dispatch spelled as an explicit cdecl call through the record word with self as the first argument
extern "C" void func_ov075_0211a194(char *self, int *src)
{
    *(int *)(self + 0x54) = src[0];
    *(int *)(self + 0x58) = src[1];
    *(int *)(self + 0x5c) = src[2];
    *(int *)(self + 0x60) = src[3];
    *(int *)(self + 0x64) = src[4];
    *(int *)(self + 0x68) = src[5];
    if (*(int *)(self + 0x54) != 0)
        vs_state_call(self, 0x54);
}

// PORT_HOST_ABI: mwcc pointer-to-member dispatch spelled as an explicit cdecl call through the record word with self as the first argument
extern "C" int func_ov075_0211a2b8(void *cv)
{
    char *cc = (char *)cv;

    if (*(unsigned char *)(cc + 0x285) != 0) {
        (*(unsigned char *)(cc + 0x284))++;
        if ((*(unsigned char *)(cc + 0x284) & 1) == 0) {
            (*(unsigned char *)(cc + 0x283))--;
            func_020551f0((void *)0x4001050, *(unsigned char *)(cc + 0x283));
            if (*(unsigned char *)(cc + 0x283) == 0) {
                *(unsigned short *)0x4001050 = 0;
                *(unsigned char *)(cc + 0x283) = 0;
                *(unsigned char *)(cc + 0x285) = 0;
            }
        }
    }

    if (data_0209f5bc->v5() == 0 || *(unsigned char *)(cc + 0x285) != 0)
        return 1;

    if (data_0209fc68 != 0) {
        if (data_0209fc68 == 6)
            func_ov075_02119dc4(cc, data_ov075_0211d930);
    }

    if (*(volatile int *)(cc + 0x5c) != 0)
        vs_state_call(cc, 0x5c);

    {
        int i = 0;
        const int n = *(unsigned char *)(cc + 0x280);
        if (n > 0) {
            char *o = cc + 0x70;
            do {
                /* the element walk: slot 0 of the record's own table (the
                   mounted data_ov075_0211c94c), self as the argument -- the
                   mwcc virtual-call shape, spelled out */
                VsStateFn *vt = *(VsStateFn **)o;
                vt[0](o);
                ++i;
                o += 0x24;
            } while (i < (int)*(unsigned char *)(cc + 0x280));
        }
    }
    return 1;
}

// PORT_HOST_ABI: mwcc pointer-to-member dispatch spelled as an explicit cdecl call through the record word with self as the first argument
extern "C" int func_ov075_0211a26c(void *cv)
{
    char *cc = (char *)cv;
    if (*(volatile int *)(cc + 0x64) != 0)
        vs_state_call(cc, 0x64);
    func_0203083c();
    return 1;
}
