#ifndef VIEW_H
#define VIEW_H

#include "dBase_c.h"
#include "math/Matrix.h"

/* The shared 0x80-byte view base used by Camera.
 *
 * RTTI and the destruction chain establish View : dBase_c. The matched Render
 * body pins the only added member: a Matrix4x3 at 0x50, immediately after the
 * 0x50-byte base. View overrides fBase_c's Render and destructor slots; it adds
 * no virtual slots of its own.
 *
 * The implicit default constructor is intentional. It lets derived constructors
 * generate the original fBase_c -> dBase_c -> View vptr sequence while leaving
 * the matrix initialization to the derived class, as Camera's C1 bytes do.
 *
 * The destructor is inline because Camera's ROM destructor inlines View's own
 * vptr store before the already-inline dBase_c teardown. A declaration alone
 * instead emits a call to ViewD2 and cannot reproduce that derived lifecycle.
 * The standalone View D1/D0 files force out-of-line copies of this inline body.
 */
struct View : dBase_c {
    Matrix4x3 viewMat; /* 0x50 */

    virtual ~View() {}
    virtual s32 Render();
};

typedef char View_size_must_be_0x80[sizeof(View) == 0x80 ? 1 : -1];

#endif /* VIEW_H */
