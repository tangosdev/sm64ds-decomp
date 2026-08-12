/* HOST COPY of src/func_ov098_02137d40.cpp -- ArrowSignRight's slot-22
 * (OnAttacked1) body.
 *
 * WHY THIS FILE EXISTS. This function's job, when it is attacked and the
 * attacker's id matches, is to dispatch its OWN vtable slot 31 (Kill). The
 * byte-locked src models the vtable as a plain array of function pointers
 * taking self explicitly:
 *
 *     c->vt->f[0x7c/4](c)      // int (*)(void *)
 *
 * which MSVC compiles __cdecl: c is PUSHED, the vtable word is loaded into a
 * register (not `this` in ecx), `call [reg+0x7c]` runs, and the CALLER cleans
 * the one pushed word (a `pop`). Slot 31 is seated in this port as __thiscall
 * with NO stack argument (as_kill / func_ov098_02137ccc, ret 0): the receiver
 * arrives in ecx and nothing is pushed. One vtable word cannot satisfy both.
 * The inner cdecl call pushes a word the thiscall callee never pops, so it
 * over-cleans against a callee that took nothing -- seating slot 22 as-is is a
 * false-fix one call deep. Until this inner site is thiscall, slot 22 stays on
 * its balanced trap (bw_trap22).
 *
 * This is the same divergence, and the same fix, that slot 28 has in
 * unmatched/Player_HeadBonk.cpp. This copy is the matched source above, line
 * for line, with exactly ONE thing changed: the inner dispatch goes through a
 * C++ virtual so MSVC emits __thiscall (receiver in ecx, no push, no caller
 * cleanup), byte for byte what slot 31's linked dispatchers emit
 * (`mov ecx,reg / mov eax,[reg] / call [eax+0x7c]`). Every constant, every
 * field offset, every branch is the byte-locked original's. src/ is untouched;
 * slice_gate33.txt carries this file in place of the .cpp, the Player_HeadBonk
 * pattern.
 *
 * WHAT IT IS IN THE GAME. A directional arrow sign in Bob-omb Battlefield. On
 * OnAttacked1 it reads the attacker's kind word (+0xc == 0xce) and, if it
 * matches, kills itself through its own slot 31 (ArrowSignRight_Kill: a
 * poof-dust + Sound::PlayBank3 + MarkForDestruction).
 */

/* The one changed line's shape. Thirty-one filler virtuals put the
   thirty-second at vtable+0x7c, which is slot 31 (0x7c/4), so the call MSVC
   emits is `call dword ptr [reg+0x7c]` with the receiver in ecx and nothing
   pushed -- byte for byte what slot 31's thiscall veneer (as_kill, ret 0)
   expects. Declared and never defined: only the call is wanted, and the object
   is always a real ArrowSignRight carrying a real vtable. */
struct KillSelf {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void Kill();   /* vtable + 0x7c */
};

extern "C" void func_ov098_02137d40(void *c, char *o)
{
    unsigned r = (*(unsigned short *)(o + 0xc) == 0xce) ? 1u : 0u;
    if (r == 0) return;
    ((KillSelf *)c)->Kill();
}
