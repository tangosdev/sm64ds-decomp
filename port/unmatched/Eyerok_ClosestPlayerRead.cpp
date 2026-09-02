/* HOST COPY of src/func_ov066_02119398.cpp -- EYEROK's (176, ov066) state
 * cell 0 TICK body, and a RECEIVER-DROPPING ClosestPlayer reader.
 * Run rel0215, lane cast-ov066.
 *
 * WHY A HOST COPY. The matched src calls
 *
 *     char* p = _ZN5Actor13ClosestPlayerEv();
 *
 * with EMPTY parentheses. Actor::ClosestPlayer is a thiscall: it reads `this`
 * from the receiver register, so a zero-argument call on the host leaves that
 * register holding whatever was there and dereferences a null base. This TU is
 * ALREADY NAMED in port/unmatched/Actor_ClosestPlayer_OverlayReaders.cpp's own
 * latent list ("src/func_ov066_02119398.cpp (ov066, calls ClosestPlayer() no
 * arg)") and in port/tools/aritycheck_receiver_baseline.txt:137 -- it was a
 * known latent reader waiting for its overlay to be mounted, and this is that
 * mount. Slicing it raw fails port/tools/closestplayer_guard.py before cmake
 * configure, which is the guard doing its job.
 *
 * THE RECEIVER IS `c`, READ OFF THE ROM RATHER THAN ASSUMED. The listing at
 * 0x02119398 in extracted/overlays/overlay_0066.bin is
 *
 *     02119398  e92d4010  push {r4, lr}
 *     0211939c  e24dd010  sub  sp, sp, #0x10
 *     021193a0  e1a04000  mov  r4, r0        <- r4 = the incoming Eyerok
 *     021193a4  ebfbddcb  bl   0x02010ad8    <- r0 STILL the incoming Eyerok
 *
 * and the reloc `from:0x021193a4 kind:arm_call to:0x02010ad8 module:main`
 * names 0x02010ad8 as _ZN5Actor13ClosestPlayerEv. r0 is untouched between the
 * prologue and the call, so ClosestPlayer's `this` rode in as this body's own
 * argument. That is the func_ov084_02129cf4 shape exactly.
 *
 * WHY IT MATTERS HERE RATHER THAN LATENTLY: this is cell 0's tick half
 * (data_ov066_0211b09c.y -> 0x02119398), so it runs every frame Eyerok spends
 * in state 0, which is the state the boss idles in.
 *
 * ONE DELIBERATE DROP: the matched source opens with
 *     struct Vec4 { int a, b, c, d; ~Vec4(){} };  Vec4 sp;
 * an unused local with a user-declared destructor, which exists only to make
 * mwcc reserve the 0x10 of stack the `sub sp, sp, #0x10` above shows. It has
 * no host meaning and is not translated; every value the body computes is
 * unchanged.
 *
 * Everything else is the matched source statement for statement, and the
 * literal pool confirms the tail: 0x02119440 -> data_ov066_0211ae08,
 * 0x02119444 -> data_ov066_0211ae04, 0x02119448 -> data_ov066_0211b0ac, and
 * the call at 0x0211942c is func_ov066_02119454, the state ENTER half.
 *
 * The matched src stays byte-locked in src/ as proof and is dropped from
 * port/slice_ov066.txt.
 *
 * PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 in from
 * this body's own argument; the host passes c).
 */
extern "C" {

void *_ZN5Actor13ClosestPlayerEv(void *self);
int func_ov066_02119454(void *c, void *p);
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211ae04;
extern int data_ov066_0211b0ac;

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 in from this body's own argument; the host passes c).
int func_ov066_02119398(char *c)
{
    char *p = (char *)_ZN5Actor13ClosestPlayerEv(c);   /* <-- this, the ROM's r0 */

    if (p != 0) {
        char *sp2 = p + 0x5c;
        int v1 = *(int *)(sp2 + 4);
        int v2 = *(int *)(sp2 + 8);
        if (v1 < -0x300000) {
            int f = (int)((*(int *)(c + 0xb0) & 8) != 0);
            if (f == 0) {
                if (v2 < -0xd70000) {
                    data_ov066_0211ae08 += 1;
                }
            }
        }
    }
    if (data_ov066_0211ae08 > 2) {
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae04 = 2;
        func_ov066_02119454(c, &data_ov066_0211b0ac);
    }
    return 1;
}

}  /* extern "C" */
