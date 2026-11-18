/* Look-at and position offset triples for the dCamera_c camera commands.
 * Six Vector3 triples (data_02086e84..data_02086ec0) that the camera-script
 * handlers pass to Vec3_RotateYAndTranslate as their base/offset.
 * Consumers: src/engine/view/d_camera.cpp, __sinit_d_camera.cpp.
 * ROM 0x02086e84..0x02086ecc, 72 bytes, 6 configured symbols.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02086eb4: 12 bytes. */
Vector3 data_02086eb4 = {0x00000000, 0x00032000, 0x00300000};

/* 0x02086ea8: 12 bytes. */
Vector3 data_02086ea8 = {0x00000000, 0x00000000, (int)0xfff00000u};

/* 0x02086e9c: 12 bytes. */
Vector3 data_02086e9c = {0x00000000, 0x00080000, 0x00000000};

/* 0x02086e90: 12 bytes. */
Vector3 data_02086e90 = {0x00000000, 0x00080000, 0x00000000};

/* 0x02086e84: 12 bytes. */
Vector3 data_02086e84 = {0x00000000, 0x00000000, (int)0xffed4000u};

/* 0x02086ec0: 12 bytes. */
Vector3 data_02086ec0 = {0x00000000, 0x00096000, (int)0xffda8000u};
