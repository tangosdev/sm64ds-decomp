// PORT_HOST_ABI. HOST COPY of func_ov006_020def80, dScMgCup_c's per-cup
// animation-set picker. Run mg9, lane CUP.
//
// ---- A MATCHED TU THAT READS AN UNINITIALISED LOCAL ------------------------
//
// src/func_ov006_020def80.c is MATCHED and byte-exact and it is WRONG ON THE
// HOST, in the class port/mg_fanout_costs.txt section 10(a) names: a defect
// that is invisible to the byte gate, invisible to the link, invisible to a
// `::*` sweep, and that only a RUN convicts. It is the first frame of scene
// 361, every time.
//
// The src declares
//
//     int new_var;
//
// assigns it `new_var = i;` on ONE of its three paths -- the last, the
// fall-through -- and then reads it after the label:
//
//     epilogue:
//       { char *s = c + (new_var << 2); s += 0x5000;
//         *((int *) (s + 0x440)) = 0;
//         *((int *) (s + 0x44c)) = 0; }
//
// The other two paths reach that label through `goto epilogue`, which jumps
// PAST the assignment. So on two of three paths the epilogue indexes the
// object by an uninitialised int.
//
// THE ROM HAS NO SUCH VARIABLE. Read out of extracted/overlays/overlay_0006.bin
// at base 0x020bfec0, the tail is
//
//     020df00c  add  r0, r0, r1, lsl #2
//     020df010  add  r0, r0, #0x5000
//     020df014  mov  r1, #0
//     020df018  str  r1, [r0, #0x440]
//     020df01c  str  r1, [r0, #0x44c]
//     020df020  bx   lr
//
// r0 is the object and r1 is `i`, and NEITHER IS WRITTEN ANYWHERE IN THE
// FUNCTION -- the whole body works in r2 and r3. Every branch falls into that
// tail with r1 still holding the parameter, so the ROM's epilogue is `i` on
// all three paths and there is no third value to name. mwccarm assigned
// new_var to r1 and the assignment compiled to nothing, which is why the src
// is a faithful transcription AND byte-identical AND unsafe to compile
// anywhere else.
//
// ---- WHAT IT DID, MEASURED -------------------------------------------------
//
// First run of scene 361 with the seat wired, SM64DS_FAULTS_FATAL=1:
//
//     walk_window crash  code c0000005  access 00000001 at f27eae10
//     +00087030 -> func_ov006_020def80 +0x70
//     +000879df -> func_ov006_020dfd48 +0x2f     (state 0)
//     +0008581e -> func_ov006_020e0204 +0xfe     (the Behavior host copy)
//     +00084fec -> cup_beh +0xc                  (vtable slot 6)
//
// -- a WRITE, not a read, which is what makes it the dangerous kind. And the
// address names the variable exactly. The object spawned at 0x307fab90 and
// MSVC left the object pointer in the register it allocated new_var to, so
//
//     c + (new_var << 2) + 0x5000 + 0x440
//   = 0x307fab90 + (0x307fab90 << 2) + 0x5440
//   = 0x307fab90 +  0xc1feae40       + 0x5440
//   = 0xf27e59d0 + 0x5440
//   = 0xf27eae10
//
// which is the faulting address to the byte. It is only a fault because the
// garbage happened to be a pointer four gigabytes out of range: a smaller
// value would have written two zero words somewhere inside the process and
// the scene would have "booted clean".
//
// ---- WHAT CHANGED, AND WHAT IT COSTS ---------------------------------------
//
// One word: `int new_var;` became `int new_var = i;`. Nothing else moves --
// every branch, every offset and every stored constant is src's, and the
// initialiser is not a guess about intent, it is what the ROM's r1 holds at
// the label on all three paths.
//
// THE COST IS ONE LINKED FUNCTION, the same trade
// unmatched/MgMemory2_ShadowSlot_020c06dc.cpp and
// unmatched/MgMemory2_ModelRender_020c1804.cpp made in run mg6: displacing a
// matched TU with a host copy means port/tools/linkage.py stops counting
// src/func_ov006_020def80.c, because this object is what the binary carries.
// port/slice_cup.txt does not list the src line. It is counted in this lane's
// linkage arithmetic rather than hidden in it, and the object name does not
// collide with any src stem, which is what port/tools/objsrc_check.py asks.
//
// THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. src/ is the byte-gated tree and
// this TU matches; whether `int new_var = i;` still compiles byte-identically
// under mwccarm 1.2/sp2p3 is a question for a matching lane, not a port one.
// If it does, this file deletes itself and the slice line comes back.
//
// THREE OTHER TUs CALL THIS BODY and all three are in port/slice_cup.txt:
// src/func_ov006_020dfd48.c (state 0, the caller in the fault above),
// src/func_ov006_020df28c.c (state 5) and src/func_ov006_020df3bc.c (state 4).
// All three declare it (receiver, int) and all three pass a real loop index,
// so nothing else needed changing.

extern "C" void func_ov006_020def80(char *c, int i);

// PORT_HOST_ABI: src leaves `int new_var;` uninitialised and reaches the epilogue via goto past its only assignment; mwcc allocated it to r1 which still holds `i` so the byte-matched body worked, but MSVC's allocation makes it a wild write, so the host initialises new_var = i, the value the ROM's r1 carries at the label on all three paths.
extern "C" void func_ov006_020def80(char *c, int i)
{
    unsigned char t;
    char *b = c + i;
    /* THE ONE CHANGED WORD. src spells `int new_var;` and assigns it only on
       the fall-through path; the ROM's r1 holds `i` at the label on every
       path. See the header. */
    int new_var = i;
    b += 0x5000;
    t = *((unsigned char *) (b + 0x465));
    if (t == 0)
    {
        t = *((unsigned char *) (b + 0x462));
        if (t != 0)
        {
            char *s = c + (i << 2);
            s += 0x5000;
            *((int *) (s + 0x434)) = 4;
        }
        else
        {
            char *s = c + (i << 2);
            s += 0x5000;
            *((int *) (s + 0x434)) = 1;
        }
        goto epilogue;
    }
    t = *((unsigned char *) (b + 0x462));
    if (t != 0)
    {
        char *s = c + (i << 2);
        s += 0x5000;
        *((int *) (s + 0x434)) = 5;
        {
            if ((*((unsigned char *) ((c + 0x5000) + 0x46d))) == 0xff)
            {
                if ((*((unsigned char *) ((c + 0x5000) + 0x469))) != 1)
                {
                    *((unsigned char *) ((c + 0x5000) + 0x46d)) =
                        (unsigned char) i;
                }
            }
        }
        goto epilogue;
    }
    {
        char *s = c + (i << 2);
        s += 0x5000;
        *((int *) (s + 0x434)) = 2;
    }
    new_var = i;
epilogue:
    {
        char *s = c + (new_var << 2);
        s += 0x5000;
        *((int *) (s + 0x440)) = 0;
        *((int *) (s + 0x44c)) = 0;
    }
}
