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
