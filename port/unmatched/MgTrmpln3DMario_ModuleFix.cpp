/* PORT_HOST_ABI.  TWO CROSS-MODULE MISBINDINGS in the element-object closure,
 * and they are the only faults that stopped scene 384 from booting.
 * Run mg11, lane TTI.
 *
 * ---- 1. WHAT THE RUN SAID ------------------------------------------------
 *
 *     FAULT code c0000005 at +0x008f2600 accessing 00cf2600
 *     port/tools/faultmap.py:
 *       +0x008f2600  data _data_ov007_020cd72c  in ov007_syms.c.obj
 *                    +0x0 into it (symbol starts at +0x008f2600)
 *
 * The program was EXECUTING at the host address of a mounted ov007 DATA object.
 * Nothing had gone wrong in the port's own code: the call site was written that
 * way in src.
 *
 * ---- 2. THE SHAPE, WHICH IS THE AMBIGUOUS-MODULE CLASS -------------------
 *
 * One DS address, two overlays, two symbols:
 *
 *     config/arm9/overlays/ov006/symbols.txt
 *         func_ov006_020cd72c kind:function(arm,size=0x18) addr:0x020cd72c
 *     config/arm9/overlays/ov007/symbols.txt
 *         data_ov007_020cd72c kind:data(any)               addr:0x020cd72c
 *
 * and src/func_ov006_020cd12c.c calls the DATA one:
 *
 *     data_ov007_020cd72c(t);
 *
 * On the DS that is not a bug that can bite: ov006 and ov007 are never resident
 * together, so the address means the ov006 function whenever this code can run.
 * THE PORT MOUNTS BOTH, so the name resolves, the link is clean, and the call
 * jumps into ov007's bytes.  It is the same class run mg9 lane S364 found at
 * 0x020ad494 (ov000's string versus ov001's dThIcon_c vtable), with the
 * difference that this one is not silent -- it is a hard crash on the first
 * element constructed, which is why scene 384 could not boot at all.
 *
 * THE ROM SETTLES IT AND THE RELOCATION IS THE WITNESS:
 *
 *     from:0x020cd134 kind:arm_call to:0x020cd72c module:overlay(6)
 *
 * -- `module:overlay(6)`, an arm_call, to a symbol ov006 calls a FUNCTION.
 *
 * ---- 3. THERE ARE TWO OF THEM AND THE SECOND IS A STATE ------------------
 *
 * A sweep of this lane's whole slice for the shape -- a `data_` symbol used as
 * a call, and any ov007 name inside an ov006 TU -- returns exactly two hits,
 * and the second is worse placed than the first:
 *
 *     src/func_ov006_020cb16c.c   `data_ov007_020ccd78()`
 *         from:0x020cb198 kind:arm_call to:0x020ccd78 module:overlay(6)
 *
 * func_ov006_020cb16c is one of the twenty-one dMgTrmpln3DMario_c STATE bodies
 * (unmatched/MgTrmpln3DMario_StateDispatch.cpp, the pair at 0x0213b20c that
 * src/func_ov006_020cb1a8.c installs), so it is on the play path rather than
 * only the construction path.
 *
 * IT ALSO DROPS THE RECEIVER.  The ROM's `bl 0x020ccd78` leaves r0 holding this
 * body's own `c`, and func_ov006_020ccd78's src takes `char *c`; the call in
 * src is spelled with no argument at all, because `data_ov007_020ccd78` is
 * declared `extern int data_ov007_020ccd78()`.  Both halves are repaired here.
 *
 * ---- 4. WHAT THIS FILE IS AND IS NOT -------------------------------------
 *
 * Both bodies below are their src verbatim with ONE call site re-pointed each
 * (and, for the second, the argument the ROM passes restored).  Nothing else
 * moved.  They take the same symbol names, so
 * src/func_ov006_020cd12c.c and src/func_ov006_020cb16c.c are OUT of
 * port/slice_tti.txt -- the unmatched/MgBSC_StateDispatch.cpp convention.
 *
 * ---- 3b. A THIRD INSTANCE, OUTSIDE EVERY SLICE --------------------------
 *
 * This lane's sweep ran over ITS OWN SLICE and found two.  The independent
 * reviewer ran it over src/ and found a third:
 *
 *     src/func_ov006_020cd6f4.c   `data_ov007_020cd72c(c)`
 *         from:0x020cd6fc kind:arm_call to:0x020cd72c module:overlay(6)
 *
 * -- the SAME wrong symbol as section 2's, in a different body, and a 0x2c
 * body whose only other relocation is a load of data_ov006_0213b3e0.  It is
 * re-derived here rather than taken on the reviewer's word.
 *
 * IT IS NOT A LIVE DEFECT ON THIS TREE and it is listed anyway.  No wired
 * slice compiles src/func_ov006_020cd6f4.c, so nothing in the port can reach
 * it today; the moment some lane's closure pulls it in, it is a jump into
 * ov007's bytes exactly like the other two were.  Scoping a sweep to your own
 * slice finds the instances that can hurt you and none of the ones that will
 * hurt the next lane.
 *
 * ---- 3c. THE ROUTED DECOMP-SIDE CORRECTION LIST, ALL THREE ---------------
 *
 *     src/func_ov006_020cd12c.c   data_ov007_020cd72c  -> func_ov006_020cd72c
 *     src/func_ov006_020cb16c.c   data_ov007_020ccd78  -> func_ov006_020ccd78
 *                                 (and the receiver it drops)
 *     src/func_ov006_020cd6f4.c   data_ov007_020cd72c  -> func_ov006_020cd72c
 *
 * THIS IS A DECOMP-SIDE CORRECTION TO ROUTE, and it is worth the three lines
 * above in whatever carries them: the byte gate cannot see it (both spellings
 * assemble to the same `bl`), and no checker in this tree looks for an ov007
 * name inside an ov006 TU.  The sweep is two greps -- a `data_` symbol used as
 * a call, and any ovNNN name inside a TU of a different overlay -- and it
 * belongs over ALL of src/, not over one slice.
 */

extern "C" {

/* the two ov006 bodies the ROM actually calls */
void func_ov006_020cd72c(int *c);
void func_ov006_020ccd78(char *c);
/* the rest of the two bodies' callees, spelled as their own src TUs spell them */
void _ZN9ModelAnimC1Ev(char *self);
extern int _ZTV18dMgTrmpln3DMario_c[];

/* ROM 0x020cd12c: the ELEMENT CONSTRUCTOR of the four dMgTrmpln3DMario_c
   records.  src/func_ov006_020cd12c.c verbatim with data_ov007_020cd72c ->
   func_ov006_020cd72c. */
int *func_ov006_020cd12c(int *t)
{
    func_ov006_020cd72c(t);
    t[0] = (int)_ZTV18dMgTrmpln3DMario_c;
    _ZN9ModelAnimC1Ev((char *)t + 0x6c);
    return t;
}

/* ROM 0x020cb16c: one of the twenty-one element STATES.
   src/func_ov006_020cb16c.c verbatim with data_ov007_020ccd78() ->
   func_ov006_020ccd78(c), the argument the ROM rides through in r0. */
void func_ov006_020cb16c(int *c)
{
    if (*(int *)((char *)c + 0x20) >= -0x120000)
        return;
    *(int *)((char *)c + 0x38) = 0;
    func_ov006_020ccd78((char *)c);
}

}  /* extern "C" */
