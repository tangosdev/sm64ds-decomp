/* PORT_HOST_ABI. func_ov006_020c0a48, the model sub-object's constructor, and
 * the reason it cannot be compiled from src: THREE OF ITS ARGUMENTS ARE
 * SPELLED AS GLOBALS THAT EXIST IN NO CONFIG. Run mg6, lane MEM.
 *
 * src/func_ov006_020c0a48.c opens
 *
 *     extern int g0[], g1[], g2[];
 *
 * and passes each by name:
 *
 *     func_02017acc(t, g0);
 *     _ZN13SharedFilePtr9ConstructEj(t + 8, g1);
 *     _ZN13SharedFilePtr9ConstructEj(t + 0x10, g2);
 *
 * There is no symbol g0 anywhere in config/, in src/ or in the ROM. They are
 * PLACEHOLDER NAMES a decompilation pass left behind for three literal-pool
 * constants, and mwccarm never had an opinion because the byte gate compiles
 * this TU on its own and matches the instruction stream, which is a pc-relative
 * load either way. On the host they are three unresolved externals -- _g0, _g1
 * and _g2 -- which is how this lane found them.
 *
 * WHAT THE ROM PASSES, read out of extracted/overlays/overlay_0006.bin at base
 * 0x020bfec0. Every one is an `ldr rN,[pc,#imm]` into the function's own pool:
 *
 *     020c0a4c  ldr r1,[pc,#0x48]   -> pool 020c0a9c = 0x00000205
 *     020c0a54  bl  func_02017acc
 *     020c0a58  ldr r1,[pc,#0x40]   -> pool 020c0aa0 = 0x00000206
 *     020c0a5c  add r0,r4,#8
 *     020c0a60  bl  SharedFilePtr::Construct
 *     020c0a64  ldr r1,[pc,#0x38]   -> pool 020c0aa4 = 0x00000207
 *     020c0a68  add r0,r4,#0x10
 *     020c0a6c  bl  SharedFilePtr::Construct
 *
 * so they are 0x205, 0x206 and 0x207 -- three consecutive NitroFS file ids, the
 * shape every other SharedFilePtr::Construct call in this family uses (its
 * sibling src/func_ov006_020c1d80.c passes 0x212, 0x210, 0x20f, 0x20c and
 * eleven more as plain literals, so the argument's KIND is not in doubt either).
 * The pool words are the only three between the function's last instruction at
 * 020c0a98 and the next symbol at 020c0aa8, so there is nothing else they could
 * be.
 *
 * THE DELTA FROM src, line by line, per the unmatched/MgBase_DeclConflict.cpp
 * precedent. The body below is src/func_ov006_020c0a48.c verbatim except:
 *
 *   1. the three `extern int g0[], g1[], g2[];` declarations are gone
 *   2. the three call sites pass the pool constants the ROM loads
 *
 * Nothing else. The BlendModelAnim constructor at +0x18, the ShadowModel
 * constructor at +0x88, the three stores at +0xe4, +0xf0 and +0xf4, and the
 * return of `t` are src's, unchanged, and every one is confirmed against the
 * disassembly above.
 *
 * IT DEFINES func_ov006_020c0a48, so src/func_ov006_020c0a48.c is OUT of
 * port/slice_mem.txt -- listing both would be an LNK2005. That is the one
 * linked function this repair costs.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN: the src TU should spell the three
 * literals rather than three invented globals, and whether it still builds
 * byte-identically under mwccarm is a byte-gated-tree question this lane does
 * not touch. It is the same routing port/mg_fanout_costs.txt section 10's
 * "fabricated initializer" defect got.
 */

extern "C" {

int func_02017acc(void *p, int id);
int _ZN13SharedFilePtr9ConstructEj(void *p, unsigned id);
int _ZN14BlendModelAnimC1Ev(void *p);
int _ZN11ShadowModelC1Ev(void *p);

// PORT_HOST_ABI: src spells three constructor arguments as placeholder globals g0/g1/g2 that resolve nowhere on the host; host copy passes the file ids 0x205/0x206/0x207 the ROM loads from its pool
int func_ov006_020c0a48(char *t)
{
    func_02017acc(t, 0x205);                       /* DELTA 2: was g0 */
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);  /* DELTA 2: was g1 */
    _ZN13SharedFilePtr9ConstructEj(t + 0x10, 0x207); /* DELTA 2: was g2 */
    _ZN14BlendModelAnimC1Ev(t + 0x18);
    _ZN11ShadowModelC1Ev(t + 0x88);
    *(int *)(t + 0xe4) = 0;
    *(short *)(t + 0xf0) = 0;
    *(int *)(t + 0xf4) = 0;
    return (int)t;
}

}  /* extern "C" */
