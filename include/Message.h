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
    u8  pad_000[0x196];
    u8  unk_196;            /* 0x196 */
#ifdef __cplusplus
    /* methods */
    void Display(unsigned int msg);

    /* Static -- no `this`; see the header note. Parameter types are read off the
       mangled name: `Et` is u16, `Eta` is (u16, s8). */
    static void PrepareTalk();
    static void EndTalk();
    static void ResetAllGlobals();
    static void DisplaySaveMenuText(u16 msgID);
    static void DisplayLevelClearText(u16 msgID, s8 course);
    static void SetTextGlobalsVS();

    /* The rest of the display entry points. `Et` is u16, `Eth` is (u16, u8),
       `Ej` is u32 -- all read off the mangled name.

       The func_0201d850 disagreement this used to record is SETTLED. Both callers
       declared that one function locally and differently -- `signed char` here,
       `unsigned char` in DisplayPauseText, both returning `int` -- and neither matched
       its definition, `void func_0201d850(u32)`. Each local declaration had been shaped
       to its own call site's argument, so no conversion was ever emitted: all five
       spellings tested byte-match at both sites, and the byte gate could not decide it.
       Settled on the definition instead, and moved to decl_common.h so there is one.

       Note the two Message methods' OWN char signedness is not part of that: `Eth` and
       `Eta` are ROM-mangled truth about these functions and stay exactly as they are. */
    static void DisplayText(u16 msgID);
    static void DisplaySaving(u16 msgID);
    static void DisplayPauseText(u16 msgID, u8 course);
    static void DisplayVsExitText(u16 msgID);
    static void DisplayPauseTextVS(u16 msgID);
    static void DisplayDontSaveText(u16 msgID);
    static void DisplaySaveStatusText(u16 msgID);
    static void DisplayOptionsMenuText(u16 msgID);
    static void DisplayControllerModeText(u16 msgID);
    static void DisplayStarNameForStarSelect(u32 star);
    static void DisplayCourseNameForStarSelect(u32 course);
    static void LoadTextVS();
#endif
};

#endif
