/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x020923a8..0x020924ac, 260 bytes, 20 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0209245c: 16 bytes. */
char data_0209245c[16] = "Ex Luigi Map";

/* 0x0209246c: 16 bytes. */
char data_0209246c[16] = "Tibi Deka In";

/* 0x0209247c: 16 bytes. */
char data_0209247c[16] = "Kaizoku Ship";

/* 0x0209248c: 16 bytes. */
char data_0209248c[16] = "Ex Wario Map";

/* 0x02092450: 12 bytes. */
char data_02092450[12] = "Koopa1 Boss";

/* 0x02092444: 12 bytes. */
char data_02092444[12] = "Desert Land";

/* 0x02092438: 12 bytes. */
char data_02092438[12] = "Snow Slider";

/* 0x0209242c: 12 bytes. */
char data_0209242c[12] = "BombHei Map";

/* 0x02092420: 12 bytes. */
char data_02092420[12] = "Koopa3 Boss";

/* 0x02092414: 12 bytes. */
char data_02092414[12] = "High Slider";

/* 0x020923fc: 12 bytes. */
char data_020923fc[12] = "Tibi Deka T";

/* 0x020923cc: 12 bytes. */
char data_020923cc[12] = "Water City";

/* 0x020923a8: 12 bytes. */
char data_020923a8[12] = "Koopa3 Map";

/* 0x020923e4: 12 bytes. */
char data_020923e4[12] = "Koopa2 Boss";

/* 0x020923d8: 12 bytes. */
char data_020923d8[12] = "Koopa2 Map";

/* 0x02092408: 12 bytes. */
char data_02092408[12] = "Clock Tower";

/* 0x020923f0: 12 bytes. */
char data_020923f0[12] = "Tibi Deka D";

/* 0x020923b4: 12 bytes. */
char data_020923b4[12] = "Koopa1 Map";

/* 0x020923c0: 12 bytes. */
char data_020923c0[12] = "Water Land";

/* 0x0209249c: 16 bytes. */
char data_0209249c[16] = "Teresa House";
