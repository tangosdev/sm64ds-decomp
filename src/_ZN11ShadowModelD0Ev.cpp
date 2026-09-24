//cpp
// @symbol _ZN11ShadowModelD0Ev
/* D0, the DELETING destructor. Unlike the D1/D2 pair these are NOT the
 * same code -- D0 runs the destructor and then hands the object to
 * operator delete, so it is longer. What is shared is the SOURCE: one
 * `ShadowModel::~ShadowModel()` makes mwcc emit D0, D1 and D2 together, and
 * objisolate keeps the one this file is bound to by config/arm9/delinks.txt.
 * That is why this file carries the same definition as
 * src/_ZN11ShadowModelD1Ev.cpp -- it is not duplication, it is how
 * one-symbol-per-file enrolment meets a compiler that emits three. */
/* recovered: real C++ destructor -- only the unlink is written by hand
 *
 * Every live ShadowModel sits on a global intrusive doubly-linked list whose
 * head is data_0209cef4; InitModel links in, RenderAll walks it, CleanAll
 * empties it, and this destructor takes one node back out. That unlink is the
 * only part of these 0x70 bytes that is really source: the vtable store at the
 * top and the ModelBase subobject call at the bottom are what `struct
 * ShadowModel : ModelBase` and `virtual ~ShadowModel()` already mean, and the
 * compiler emits them around the body.
 *
 * The list is singly-headed, so removing the first node is the case that needs
 * the head compared against `this`; every other node is reached through its
 * predecessor. Clearing both links afterwards is what lets CleanAll use a null
 * `next` as its terminator.
 *
 * The unlink is also the layout evidence for the two pointers: it is what pins
 * prev at 0x20 and next at 0x24.
 */
#include "ShadowModel.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1ShadowModel@@QAE@XZ that
 * src/_ZN11ShadowModelD1Ev.cpp defines out of line (include/ShadowModel.h
 * declares ~ShadowModel() with no slot under _MSC_VER), so this file cannot
 * carry the same definition: the host link refuses the pair (LNK2005). It
 * spells out, in terms of that one host symbol, what the variant this file is
 * enrolled for does: the D1 body, then the class-specific operator delete
 * (Memory::operator_delete2, from ModelBase's inline operator delete). The
 * qualified call is direct. ShadowModel::Destructor0 (hal/int4_rows.cpp, the
 * host vtable's slot 1) calls this name. Nothing here reaches mwccarm: it
 * builds the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" ShadowModel *_ZN11ShadowModelD0Ev(ShadowModel *thiz)
{
    thiz->ShadowModel::~ShadowModel();          /* the D1 body, through the one host symbol */
    ShadowModel::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
extern ShadowModel *data_0209cef4;  /* head of the live-shadow list */

ShadowModel::~ShadowModel()
{
    if (prev)
        prev->next = next;
    else if (data_0209cef4 == this)
        data_0209cef4 = next;

    if (next)
        next->prev = prev;

    prev = 0;
    next = 0;
}
#endif
