#ifndef DAOBJKUMO_C_H
#define DAOBJKUMO_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Cloud platform (ov039 CLOUD / OBJ_KUMO, actor 314). `kumo` is the ROM's
 * own word for it -- the debug table and _ZTS11daObjKumo_c both say so --
 * not a spider.
 *
 *   _ZTS  ov039 0x02111824  "11daObjKumo_c"
 *   _ZTI  ov039 0x02111818  __si_class_type_info; +8 -> _ZTI8dActor_c
 *                           (arm9 0x0208e390), so the DIRECT base is
 *                           dActor_c and nothing else.
 *   _ZTV  ov039 0x02111858  the ADDRESS POINT: V-8 is a zero offset-to-top,
 *                           V-4 is &_ZTI, V+0 is slot 0 (InitResources).
 *   size  0x124             daObjKumo_c_classInit's own literal (292).
 *
 * TWO WITNESSES, and they close on each other:
 *   factory  fBase_c::operator new(292), dActor_c::dActor_c(), stores
 *            _ZTV11daObjKumo_c, then Model at 0xd4.
 *   dtor     the same member destroyed in reverse, then ~dActor_c.
 *
 * pad_0d0[0x4] IS AN UNKNOWN FIELD, NOT ALIGNMENT. dActor_c asserts 0xd0 and
 * Model needs no more than 4-byte alignment; nothing in this TU reads or
 * writes those four bytes.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots
 * declared below differ; every other slot holds the base's own word and is
 * inherited, so it is deliberately not redeclared here.
 */
struct daObjKumo_c : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;            /* 0x0d4 */

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjKumo_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual int InitResources();       /* slot  0 */
    virtual int CleanupResources();    /* slot  3 */
    virtual int Behavior();            /* slot  6 */
    virtual int Render();              /* slot  9 */

    /* size_t == unsigned long here; unsigned int is illegal. Forwards to
       fBase_c::operator new until #2570 merges a shared spelling. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjKumo_c_size_must_be_0x124[sizeof(daObjKumo_c) == 0x124 ? 1 : -1];

#endif /* DAOBJKUMO_C_H */
