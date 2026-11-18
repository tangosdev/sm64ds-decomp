/* Per-player health and the control-mode button maps.
 * gPlayerHealth[4] holds the four players' health words, and data_0209214c
 * points at the three 16-entry u16 button maps data_02075650/670/690 that
 * d_s_stage_methods applies per control mode.
 * Consumers: d_a_ply_methods (health), d_s_stage_methods (button maps).
 * ROM 0x02092144..0x02092158, 20 bytes, 2 configured symbols.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

extern u8 data_02075650[];
extern u8 data_02075670[];
extern u8 data_02075690[];

/* 0x02092144: 8 bytes. */
short gPlayerHealth[4] = {
    0x0880,
    0x0880,
    0x0880,
    0x0880,
};

/* 0x0209214c: 12 bytes. */
unsigned short *data_0209214c[3] = {
    (unsigned short *)&data_02075650,
    (unsigned short *)&data_02075670,
    (unsigned short *)&data_02075690,
};
