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
#endif
};

#endif
