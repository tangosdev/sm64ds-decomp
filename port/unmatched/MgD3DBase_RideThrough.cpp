/* PORT_HOST_ABI.  dScMgD3DBase_c's host copies: TWO ride-through repairs and
 * ONE body the host compiler will not accept from src at all.
 * Run mg11, lane TTI.  None of the three is a decompilation and none claims to
 * be one: the first two are the host's calling convention being placed by hand,
 * on bodies whose src is a correct reading of an ARM function and a wrong
 * program on x86, and the third is a re-spelling of a body MSVC refuses.
 *
 * THE SHAPE, WHICH IS port_mg_flower_after_init's EXACTLY.  On ARM an argument
 * a function never names is still in its register when that function calls
 * something that does name it.  A C decompilation cannot spell that, so the
 * argument disappears from the source and reappears from the calling
 * convention.  On the host there is no register to ride: the callee reads
 * whatever the host left on the stack.
 *
 * ---- 1. SLOT 2, AfterInitResources, ROM 0x020e70c0 ------------------------
 *
 * Disassembled out of extracted/overlays/overlay_0006.bin at base 0x020bfec0,
 * 8 instructions plus a one-word pool = 9 words = 0x24, which is the size
 * config records:
 *
 *     push {r4,lr} / mov r4,r0
 *     bl   0x020b08f0            <- r1 NEVER WRITTEN: the flags ride through
 *     ldr  r0,[pc,#0xc] (= 0x000047e4) / add r0,r4,r0
 *     bl   0x02022f40            <- Particle::SysTracker::Initialise
 *     pop  {r4,lr} / bx lr
 *
 * src/_ZN17MgBounceAndPounce18AfterInitResourcesEj.cpp reads
 *
 *     func_ov004_020b08f0(((void*)self));
 *
 * with one argument, and hal/scene_mg.cpp already declares that body's real
 * shape -- `void func_ov004_020b08f0(void *c, unsigned f)` -- because
 * dScMgBase_c's own slot 2 takes the flags.  So this repair is
 *
 *     src:   int  _ZN17MgBounceAndPounce18AfterInitResourcesEj(self)
 *              func_ov004_020b08f0(self);
 *     here:  int  port_mg_d3d_after_init(void *c, unsigned f)
 *              func_ov004_020b08f0(c, f);
 *
 * and nothing else.  The Particle::SysTracker::Initialise call, the +0x47e4
 * offset and the return of its result are src's, verbatim.
 *
 * ---- 2. SLOT 10, BeforeRender, ROM 0x020e7040 -----------------------------
 *
 * 13 instructions, no pool = 0x34, which is the size config records:
 *
 *     stmdb sp!,{lr} / sub sp,sp,#4
 *     bl   0x020b04f4            <- r0 UNTOUCHED FROM ENTRY: `this` itself
 *     cmp  r0,#0 / <return 0 arm>
 *     bl   0x02022ee0            <- Particle::RenderAll
 *     mov  r0,#1 / <return 1>
 *
 * src/_ZN17MgBounceAndPounce12BeforeRenderEv.cpp reads
 *
 *     extern int func_ov004_020b04f4();
 *     if(func_ov004_020b04f4()==0) return 0;
 *
 * with NO parameter on either side, so on the host dScMgBase_c::BeforeRender is
 * handed whatever the caller's frame happens to hold as its `this`.  That is
 * the DROPPED RECEIVER the run law calls the bin-softlock disease, and it is
 * worse than the slot-2 case because the lost value is the object itself.
 * hal/scene_mg.cpp already declares the callee's real shape,
 * `int func_ov004_020b04f4(void *c)`, for the same reason.
 *
 *     src:   int  _ZN17MgBounceAndPounce12BeforeRenderEv()
 *              func_ov004_020b04f4();
 *     here:  int  port_mg_d3d_before_render(void *c)
 *              func_ov004_020b04f4(c);
 *
 * Particle::RenderAll takes no argument in src and the ROM passes it the
 * previous call's return value in r0, which it cannot read as a parameter it
 * does not have.  It is left exactly as src has it.
 *
 * ---- 3. WHY THE TWO src TUs STAY OUT OF THE SLICE -------------------------
 *
 * These carry port_ names and do NOT define
 * _ZN17MgBounceAndPounce18AfterInitResourcesEj or
 * _ZN17MgBounceAndPounce12BeforeRenderEv.  The decomp has a real matched body
 * for each and this file is not a stand-in for either.  So both src TUs stay
 * out of port/slice_tti.txt, both symbols stay absent from the port's map, and
 * nothing in this tree claims a decompilation that does not exist -- the ruling
 * port/unmatched/MgFlower_Slot2.cpp made for the same shape.
 *
 * THE BODIES ARE dScMgD3DBase_c's SHARED GLUE, NOT dScMgTrampoline_c's, and
 * the file is named for the base rather than for the lane that paid for it.
 * FOUR classes derive from that base -- dScMgJump_c (0x174), dScMgJump2_c
 * (0x175), dScMgTrampoline_c (0x180) and dScMgTrampoline2_c (0x181) -- and
 * three more lanes are seating the other three in this same run.  Every one of
 * them inherits slots 2 and 10 unchanged.  THEY SHOULD CALL THESE RATHER THAN
 * WRITE A SECOND COPY: a second definition of either name is an LNK2005, and
 * two host copies over the same two DS words would split the per-slot witness.
 *
 * ---- 4. SLOT 24, ROM 0x020e6e78: THE HEADER AND THE BODY DISAGREE --------
 *
 * This one is not an ABI repair.  src/func_ov006_020e6e78.cpp is a faithful
 * decompilation -- it was disassembled and ruled REAL_DECOMP by this lane, and
 * the ruling is in port/tools/inferred_stub_adjudicated.txt -- but the port
 * cannot compile it:
 *
 *     src\func_ov006_020e6e78.cpp(13): error C2733: 'func_ov006_020e6e78':
 *     you cannot overload a function with 'extern "C"' linkage
 *
 * because include/decl_common.h opens `extern "C" {` at line 20 and declares
 *
 *     extern int func_ov006_020e6e78(void*);        line 1516
 *
 * while the TU that includes it defines
 *
 *     extern "C" int func_ov006_020e6e78(char* self);
 *
 * void* and char* are different parameter types, so the two are overloads of
 * one extern "C" name.  mwcc accepted the pair; MSVC will not, and it is right
 * to refuse -- a C symbol cannot have two signatures.  THIS IS A DECOMP-SIDE
 * DEFECT worth routing: the shared header and the body disagree about the same
 * symbol, and nothing in the decomp's own build notices because the two
 * spellings compile to the same ARM.
 *
 * The body below is that file verbatim, with decl_common.h not included and its
 * four externs spelled locally instead, and NOTHING ELSE MOVED: the same guard,
 * the same in-place toggle of the +0x4664 halfword, the same 0xbc stride, the
 * same two arms and the same return values.  It takes the same symbol name --
 * the unmatched/MgBSC_StateDispatch.cpp convention for "the port cannot compile
 * the src" -- so src/func_ov006_020e6e78.cpp stays out of port/slice_tti.txt.
 *
 * It is dScMgD3DBase_c's slot 24 and therefore SHARED FOUR WAYS, which is why
 * it is in this file and not in a per-class one.
 */

extern "C" {

void func_ov004_020b08f0(void *c, unsigned f);
int  func_ov004_020b04f4(void *c);
int  _ZN8Particle10SysTracker10InitialiseEv(void *);
/* void, matching src/_ZN8Particle9RenderAllEv.c and hal/particle_bridges.cpp.
   The src TU this file re-spells declares it int and discards the value, which
   is a decompiler's spelling of the ROM's `bl` with nothing after it. */
void _ZN8Particle9RenderAllEv(void);

/* ROM 0x020e70c0, dScMgD3DBase_c vtable slot 2 */
int port_mg_d3d_after_init(void *c, unsigned f)
{
    func_ov004_020b08f0(c, f);
    return _ZN8Particle10SysTracker10InitialiseEv((char *)c + 0x47e4);
}

/* ROM 0x020e7040, dScMgD3DBase_c vtable slot 10 */
int port_mg_d3d_before_render(void *c)
{
    if (func_ov004_020b04f4(c) == 0)
        return 0;
    _ZN8Particle9RenderAllEv();
    return 1;
}

/* ---- section 4: ROM 0x020e6e78, dScMgD3DBase_c vtable slot 24 ------------
   src/func_ov006_020e6e78.cpp verbatim.  decl_common.h is deliberately NOT
   included -- including it is what makes the TU uncompilable -- so its four
   externs are spelled here.  The declaration below uses char*, which is what
   the ROM body's own src uses and what the pointer arithmetic needs. */
int  func_ov004_020ae140(void *self);
void Camera_UpdateMatrices(int arg);
int  func_ov006_020e7508(void);
int  func_ov006_020e759c(void);
extern unsigned char data_0209f5f8;
extern unsigned char data_0209d464;
extern int data_ov006_02141a44;

int func_ov006_020e6e78(char *self)
{
    if (func_ov004_020ae140(self) == 0) return 0;
    if (*(int *)(self + 0x4628) == 0) {
        if (data_0209d464 == 0) return 0;
        if (*(unsigned short *)(self + 0x4664) == 0)
            *(unsigned short *)(self + 0x4664) = 1;
        else
            *(unsigned short *)(self + 0x4664) = 0;
        int v = (int)(self + 0x466c + (*(unsigned short *)(self + 0x4664)) * 0xbc);
        data_ov006_02141a44 = v;
        Camera_UpdateMatrices(v);
        if (*(unsigned short *)(self + 0x4664) == 1) {
            func_ov006_020e7508();
        } else {
            func_ov006_020e759c();
            if (data_0209f5f8 == 0) {
                *(int *)0x4001000 &= ~0xe000;
                data_0209f5f8 = 1;
            }
        }
    }
    return 1;
}

}  /* extern "C" */
