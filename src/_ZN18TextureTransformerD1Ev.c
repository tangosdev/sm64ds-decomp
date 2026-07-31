// @symbol _ZN18TextureTransformerD1Ev
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "TextureTransformer.h"
/* _ZN18TextureTransformerD1Ev at 0x0201592c
 * Single-vtable destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call base/helper destructor, return this.
 * Call target: 0x02015cb4
 */
struct Obj { void *vtable; };
extern void base_dtor_TextureTransformer(struct Obj *thiz); /* 0x02015cb4 */
struct Obj *_ZN18TextureTransformerD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)vtbl_TextureTransformer;
    base_dtor_TextureTransformer(thiz);
    return thiz;
}
