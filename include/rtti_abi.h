/* The ABI typeinfo vtable every Itanium `_ZTI` record names as its first word.
 *
 * mwcc emits a `_ZTI` record as three words: this vtable address, the class's
 * `_ZTS` name, and the base class's `_ZTI` (or zero). The ROM owns the
 * vtable's storage, so data records only need the declaration, kept here
 * instead of repeated in every typeinfo file.
 */
#ifndef RTTI_ABI_H
#define RTTI_ABI_H
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];

#endif
