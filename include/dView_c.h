#ifndef DVIEW_C_H
#define DVIEW_C_H

#include "dBase_c.h"
#include "math/Matrix.h"

extern "C" void Matrix4x3_LoadIdentity(Matrix4x3 *matrix);

/* The shared 0x80-byte view base used by dCamera_c.
 *
 * RTTI names the class: _ZTS7dView_c at 0x02086e48 reads "7dView_c" and
 * _ZTI7dView_c at 0x02086ecc is an __si_class_type_info on dBase_c. The
 * matched Render body pins the only added member: a Matrix4x3 at 0x50,
 * immediately after the 0x50-byte base. dView_c overrides fBase_c's Render
 * and destructor slots; it adds no virtual slots of its own.
 *
 * The constructor clears the view matrix. That call sits between the dView_c
 * vtable store and the derived class's vtable store, which is where
 * dCamera_c's factory leaves it.
 *
 * The destructor is inline because dCamera_c's ROM destructor inlines dView_c's
 * own vptr store before the already-inline dBase_c teardown. A declaration
 * alone instead emits a call to ViewD2 and cannot reproduce that derived
 * lifecycle. The TU's emit-forcers pin the out-of-line D1/D0 copies.
 */
struct dView_c : dBase_c {
    Matrix4x3 viewMat; /* 0x50 */

    dView_c() {
        Matrix4x3_LoadIdentity(&viewMat);
    }
    virtual ~dView_c() {}
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dView_c_size_must_be_0x80[sizeof(dView_c) == 0x80 ? 1 : -1];
#endif

#endif /* DVIEW_C_H */
