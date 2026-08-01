// @symbol _ZN15TextureSequenceD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "TextureSequence.h"
/* _ZN15TextureSequenceD1Ev at 0x02015a2c
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x02015cb4
 */
struct Obj { void *vtable; };
extern void base_dtor_TextureSequence(struct Obj *thiz); /* 0x02015cb4 */
struct Obj *_ZN15TextureSequenceD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_TextureSequence;
    base_dtor_TextureSequence(thiz);
    return thiz;
}
