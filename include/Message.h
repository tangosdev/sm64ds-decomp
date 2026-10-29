/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2.
 *
 * class Message: 15 matched functions, 1 evidenced field.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * Message is NOT polymorphic -- no RTTI record, no vtable, no ctor or dtor -- so
 * the key-function rule (runbook section 7) does not apply and its methods migrate
 * freely. They are STATIC: the ROM bodies take no `this` and drive a set of file
 * globals around 0x0209d660. */
#ifndef MESSAGE_H
#define MESSAGE_H
#include "types.h"

/* fwd */
struct msg;
struct Message {
    u8 pad_000[0x196];
    u8 unk_196; /* 0x196 */
#ifdef __cplusplus
    /* methods */

    /* Static -- no `this`; see the header note. Parameter types are read off the
       mangled name: `Et` is u16, `Eth` is (u16, u8). */
    static void Display(u32 msg);
    static void PrepareTalk();
    static void EndTalk();
    /* The parameter is unnamed on purpose. In mwccarm a named parameter in a
       class-scope declaration takes a number from the counter that names the
       compiler's @NNN temporaries, so naming it would renumber every TU
       that includes this header, and the TU manifests pin those numbers. */
    static void AddChar(u8);
    static void AddCharVS(u8);
    static int UpdateWindow();
    static void Update();
    static void ResetAllGlobals();
    static void DisplaySaveMenuText(u16 msgID);
    static void DisplayLevelClearText(u16 msgID, u8 course);
    static void SetTextGlobals();
    static void SetTextGlobalsVS();
    static void LoadText();

    /* The rest of the display entry points. `Et` is u16, `Eth` is (u16, u8),
       `Ej` is u32 -- all read off the mangled name.

       The func_0201d850 disagreement this used to record is SETTLED. Both callers
       declared that one function locally and differently -- `signed char` here,
       `unsigned char` in DisplayPauseText, both returning `int` -- and neither matched
       its definition, `void func_0201d850(u32)`. Each local declaration had been shaped
       to its own call site's argument, so no conversion was ever emitted: all five
       spellings tested byte-match at both sites, and the byte gate could not decide it.
       Settled on the definition instead, and moved to decl_common.h so there is one.

       The two Message methods' OWN char parameter is a separate question. Both are
       `Eth` (u8): DisplayLevelClearText used to be spelled `Eta` (s8), but its one
       caller narrows the course with `and 0xff`, which only a u8 parameter emits. */
    static void DisplayText(int msgID);
    static void DisplaySaving(u16 msgID);
    static void DisplayPauseText(u16 msgID, u8 course);
    static void DisplayVsExitText(u16 msgID);
    static void DisplayPauseTextVS(u16 msgID);
    static void DisplayDontSaveText(u16 msgID);
    static void DisplayDontSaveTextVS(u16);
    static void DisplaySaveStatusText(u16 msgID);
    static void DisplayOptionsMenuText(s16 msgID);
    static void DisplayOptionsMenuTextVS(s16);
    static void DisplayControllerModeText(u16 msgID);
    static void DisplayStarNameForStarSelect(u32 star);
    static void DisplayCourseNameForStarSelect(u32 course);
    static void LoadTextVS();
#endif
};

#endif
