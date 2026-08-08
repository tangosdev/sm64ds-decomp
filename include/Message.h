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

       NOTE the disagreement this exposes: DisplayLevelClearText passes its course
       to func_0201d850 declared `signed char`, while DisplayPauseText declares the
       same function `unsigned char`. One function, two signatures, which is the
       declaration debt notes/declaration-centralization.md measures at 27%. Both
       reproduce, because the byte gate cannot see it. Not settled here. */
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
