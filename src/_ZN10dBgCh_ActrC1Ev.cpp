//cpp
// @symbol _ZN10dBgCh_ActrC1Ev
/* recovered: real C++ constructor -- EMPTY BODY. The ROM's four steps are all
 * synthesis, in exactly the measured emission order (notes/ctor-migration.md
 * section 6):
 *
 *   bl _ZN5dBgChC2Ev          the dBgCh base step at +0x00
 *   vptr = _ZTV10dBgCh_Actr   the derived primary block over +0x00
 *   bl _ZN12dBgCh_SphCrrC1Ev  member mSphereClsn at +0x20
 *   bl _ZN9dBgCh_LinC1Ev      member mRaycastLine at +0x134
 *
 * What made this possible was typing the two members as themselves
 * (include/dBgCh_Actr.h): a class whose sub-object members have declared,
 * out-of-line constructors gets every one of those calls for free. Nothing
 * derives from dBgCh_Actr, so this is the only variant the ROM carries.
 */
#include "dBgCh_Actr.h"

dBgCh_Actr::dBgCh_Actr() {}
