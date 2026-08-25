/* HOST FACE for func_0203dabc, THE COMMS SLOT INDEX. Run mg12, lane TITLE.
 *
 * ============================ THE DEFECT ==================================
 *
 * src/func_0203dabc.c is MATCHED and stays matched. It is displaced from the
 * PORT only, and the reason is the arity ride-through this tree already has a
 * name and a baseline for (port/tools/aritycheck_plainfunc_baseline.txt lines
 * 80 and 81, both citing include/decl_common.h).
 *
 *     include/decl_common.h:1750     extern u16 *func_0203dabc(void);
 *     src/func_0203dabc.c:3          int *func_0203dabc(int idx)
 *
 * The declaration takes nothing and the definition takes an index. On ARM that
 * is not a bug, it is the calling convention doing its job: the ROM's only
 * caller is
 *
 *     src/func_ov007_020cc2cc.c:28-30
 *         func_0203da9c();
 *         u16 *p = func_0203dabc();
 *
 * and func_0203da9c is `return data_020a0f10;` (src/func_0203da9c.c), so r0
 * already holds the index when the next `bl` runs and the callee reads its own
 * argument out of it. On x86 nothing is pushed and the callee reads whatever
 * the caller left at [esp+4]: a stack word, used as an index into
 * data_020a1154 at stride 0x24. Every frame the title screen is up.
 *
 * ============================ THE FACE ====================================
 *
 * This body takes no argument and reads data_020a0f10 ITSELF, which is exactly
 * what the ROM's r0 holds at that point. It is the same shape as
 * port/unmatched/Ov007_ImplicitR0_020add3c.cpp: the matched TU keeps its
 * bytes, the port gets a frame whose declaration and definition agree.
 *
 * SAFE BECAUSE THERE IS EXACTLY ONE LINKED CALLER, and that was measured
 * rather than assumed. The whole tree has three callers of this address:
 * src/func_ov007_020cc2cc.c, src/func_0203bb60.c and src/func_0203bc7c.c. The
 * last two declare it with an int parameter of their own -- they would pass a
 * real index -- and NEITHER IS IN ANY SLICE, so neither is in any binary this
 * repo builds. The day one of them is linked, this face is wrong for it and
 * the fix is the header, not this file.
 *
 * data_020a0f10 is `int data_020a0f10[8]` in port/hal/actor_vtables.cpp and is
 * zero, which is the DS's own value on a single-player boot: the comms slot
 * index only moves in the wireless paths (src/func_0203db64.c sets it back to
 * 0). So this resolves to slot 0 today and will follow the global if anything
 * ever moves it.
 *
 * NOT A DECOMP AND NOT SCORED. The stem deliberately does not spell
 * func_0203dabc, so port/tools/objsrc_check.py cannot read this as the matched
 * TU and port/tools/linkage.py does not count it. It carries no
 * "recovered from vtable slot identity" marker because it is not that kind of
 * body.
 */

extern "C" {

extern int data_020a0f10[];

struct Ov007CommsRecord { char _f0[4]; int f4; char _rest[0x24 - 8]; };
extern Ov007CommsRecord data_020a1154[];

int *func_0203dabc(void);

int *func_0203dabc(void)
{
    /* src/func_0203dabc.c's body, with the argument the ROM rides in on read
       from the global the ROM's own accessor returns. */
    return &data_020a1154[data_020a0f10[0]].f4;
}

}
