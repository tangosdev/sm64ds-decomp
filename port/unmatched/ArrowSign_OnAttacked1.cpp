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
 * arrives in ecx and nothing is pushed. One vtable word cannot satisfy both,
 * and seating slot 22 on the raw src is a false-fix one call deep. Until this
 * inner site is thiscall, slot 22 stays on its balanced trap (bw_trap22).
 *
 * WHAT EXACTLY BREAKS (corrected 2026-08-12, run linkw lane l1: an earlier
 * reading of this file called it a stack-cleanup mismatch, and the listing
 * says otherwise). The stack does balance -- the raw src pushes one word and
 * pops it itself, and as_kill's `ret 0` leaves it there to pop. What breaks is
 * the RECEIVER. MSVC loads the vtable pointer into ecx to reach the slot, so
 * ecx holds the VTABLE, not the object, when the call lands; as_kill is
 * __fastcall and reads `this` out of ecx, so ArrowSignRight_Kill would run
 * against _ZTV14ArrowSignRight and read the sign's fields out of vtable words.
 * A wrong-receiver call, not a leaked word.
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
 *
 * PROOF (run linkw, lane l1). Both spellings compiled standalone with the port's
 * own cl, /O2 /FAsc; the whole body is short enough to quote entire.
 * src/func_ov098_02137d40.cpp:
 *
 *     ; c->vt->f[0x7c/4](c);
 *       8b 44 24 04  mov  eax, DWORD PTR _c$[esp-4]
 *       50           push eax          ; c, pushed
 *       8b 08        mov  ecx, DWORD PTR [eax]   ; ecx = the VTABLE
 *       8b 41 7c     mov  eax, DWORD PTR [ecx+124]
 *       ff d0        call eax
 *       59           pop  ecx
 *
 * this file:
 *
 *     ; ((KillSelf *)c)->Kill();
 *       8b 4c 24 04  mov  ecx, DWORD PTR _c$[esp-4]   ; ecx = the OBJECT
 *       8b 01        mov  eax, DWORD PTR [ecx]
 *       ff 60 7c     jmp  DWORD PTR [eax+124]         ; tail call, nothing pushed
 *
 * ecx holds the vtable in the first and the object in the second, and slot 31's
 * seat reads `this` from ecx (hal/actor_classes_bob_world.cpp:1139
 * `static int __fastcall as_kill(void *s, void *)`, seated at :1190). That is
 * the defect: not a lost word, a wrong `this`. On ARM both spellings are
 * `ldr r1,[r0] / ldr pc,[r1,#0x7c]` with the object already in r0, which is why
 * the transcription had no reason to prefer either.
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

/* PORT_HOST_ABI: cdecl-vs-thiscall vtable dispatch -- the raw src's inner slot-31 call leaves the VTABLE POINTER in ecx and pushes `this` (`push eax / mov ecx,[eax] / call [ecx+0x7c] / pop ecx`), while the seated slot-31 veneer as_kill is __fastcall and reads `this` from ecx (hal/actor_classes_bob_world.cpp:1139,1190). Wrong receiver, not a lost word; listings in the block above. */
/* func_ov098_02137d40 RETIRED (run link100, lane SEAT6, batch B6).
   Lane FACEF called the fix decomp-side ('only the matched TU's own
   spelling can put `this` in ecx'). The generated TU's spelling does
   exactly that, and src/ does not move.
   The matched TU src/func_ov098_02137d40.cpp is seated in its place: port/tools/hostgen.py's VIRTUAL_CALL table calls word 31
   __fastcall(c, 0), which is where as_kill reads its `this` from.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
