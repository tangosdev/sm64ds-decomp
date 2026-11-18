/* Look-at and position offset triple for the dCamera_c camera commands.
 * data_02086ed8 is the last Vector3 of the triple group that starts at
 * data_02086e84.
 * Consumers: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086ed8..0x02086ee4, 12 bytes, 1 configured symbol.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02086ed8: 12 bytes. */
Vector3 data_02086ed8 = {0x00258000, 0x00000000, (int)0xffe70000u};
