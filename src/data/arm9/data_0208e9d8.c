/* Destructor thunks of the dExtAnm2Model_c frame-control subobject.
 * The two words are the code addresses of the adjusted-base destructor pair
 * _ZThn80_N15dExtAnm2Model_cD1Ev/_ZThn80_N15dExtAnm2Model_cD0Ev.
 * Consumers: src/engine/model/d_ext_model.cpp.
 * ROM 0x0208e9d8..0x0208e9e0, 8 bytes, 1 configured symbol.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

extern void _ZThn80_N15dExtAnm2Model_cD1Ev();
extern void _ZThn80_N15dExtAnm2Model_cD0Ev();

/* 0x0208e9d8: 8 bytes. */
u32 VTable_dExtFrameCtrl_c_dExtAnm2Model_cThunk[2] = {
    (u32)&_ZThn80_N15dExtAnm2Model_cD1Ev,
    (u32)&_ZThn80_N15dExtAnm2Model_cD0Ev,
};
