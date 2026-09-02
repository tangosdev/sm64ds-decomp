/* PORT_HOST_ABI. func_ov006_020c221c, the +0x4f38 sub-object's constructor,
 * and the reason it cannot be compiled from src: TWO OF ITS ARGUMENTS ARE
 * SPELLED AS GLOBALS THAT EXIST IN NO CONFIG. Run mg11, lane MUG.
 *
 * This is the SAME defect unmatched/MgMemory2_SubObjCtor.cpp records for
 * func_ov006_020c0a48, on a sibling body in the same family, and that file's
 * header is the derivation. Repeating the measurement rather than citing it,
 * because the constants are this body's own:
 *
 *     src/func_ov006_020c221c.c opens `extern int g0[], g1[];` and passes each
 *     by name:
 *
 *         func_02017acc(t, g0);
 *         _ZN13SharedFilePtr9ConstructEj(t + 8, g1);
 *
 *     There is no symbol g0 or g1 anywhere in config/, in src/ or in the ROM.
 *     They are PLACEHOLDER NAMES a decompilation pass left behind for two
 *     literal-pool constants, and mwccarm never had an opinion because the
 *     byte gate compiles this TU on its own and matches the instruction
 *     stream, which is a pc-relative load either way. On the host they are two
 *     unresolved externals, _g0 and _g1, which is how this lane found them --
 *     the same way lane MEM found its three.
 *
 * WHAT THE ROM PASSES, read out of extracted/overlays/overlay_0006.bin at base
 * 0x020bfec0 (size 0x40 from config/arm9/overlays/ov006/symbols.txt):
 *
 *     020c221c  push  {r4, lr}
 *     020c2220  ldr   r1,[pc,#0x2c]   -> pool 020c2254 = 0x00000205
 *     020c2224  mov   r4, r0
 *     020c2228  bl    #0x2017acc
 *     020c222c  ldr   r1,[pc,#0x24]   -> pool 020c2258 = 0x00000206
 *     020c2230  add   r0, r4, #8
 *     020c2234  bl    #0x201799c      SharedFilePtr::Construct
 *     020c2238  add   r0, r4, #0x10
 *     020c223c  bl    #0x20166d4      BlendModelAnim::BlendModelAnim
 *     020c2240  mov   r1, #0
 *     020c2244  mov   r0, r4
 *     020c2248  str   r1, [r4, #0xa8]
 *     020c224c  pop   {r4, lr}
 *     020c2250  bx    lr
 *
 * so they are 0x205 and 0x206, TWO CONSECUTIVE NitroFS file ids -- and they are
 * the SAME first two ids func_ov006_020c0a48 passes, which is the strongest
 * available check that the reading is right: two different bodies in the same
 * sub-object family, disassembled independently, land on the same shared
 * model and animation files. The two pool words are the only two between this
 * function's last instruction at 0x020c2250 and the next symbol at 0x020c225c,
 * so there is nothing else they could be.
 *
 * THE DELTA FROM src, line by line. The body below is src/func_ov006_020c221c.c
 * verbatim except:
 *
 *   1. the `extern int g0[], g1[];` declaration is gone
 *   2. the two call sites pass the pool constants the ROM loads
 *
 * Nothing else. The BlendModelAnim constructor at +0x10, the store of 0 at
 * +0xa8 and the return of `t` are src's, unchanged, and every one is confirmed
 * against the disassembly above.
 *
 * IT DEFINES func_ov006_020c221c, so src/func_ov006_020c221c.c is OUT of
 * port/slice_mug.txt -- listing both would be an LNK2005. That is the one
 * linked function this repair costs.
 *
 * ONE CALLER, AND IT IS THIS LANE'S. config/arm9/overlays/ov006/relocs.txt
 * carries exactly one arm_call to 0x020c221c, from 0x0210c15c inside
 * func_ov006_0210c120 -- dScMgSlot3_c's factory. So the file is lane-named
 * rather than shared: if a later class turns out to construct the same
 * sub-object, the canonical-copy question is theirs and the answer is this
 * file, the way unmatched/MgShared4f38_SubDispatch.cpp became the canonical
 * copy of func_ov006_020c2b8c at the mg9 merge.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN: the src TU should spell the two
 * literals rather than two invented globals, and whether it still builds
 * byte-identically under mwccarm is a byte-gated-tree question this lane does
 * not touch. Same routing lane MEM gave its three.
 */

extern "C" {

int func_02017acc(void *p, int id);
int _ZN13SharedFilePtr9ConstructEj(void *p, unsigned id);
int _ZN14BlendModelAnimC1Ev(void *p);

// PORT_HOST_ABI: src passes two invented placeholder globals g0/g1 that exist in no config, src or ROM and are unresolved externals under MSVC; the host copy passes the two consecutive NitroFS file-id immediates 0x205 and 0x206 the ROM's literal pool actually loads.
int func_ov006_020c221c(char *t)
{
    func_02017acc(t, 0x205);                       /* DELTA 2: was g0 */
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);  /* DELTA 2: was g1 */
    _ZN14BlendModelAnimC1Ev(t + 0x10);
    *(int *)(t + 0xa8) = 0;
    return (int)t;
}

}  /* extern "C" */
