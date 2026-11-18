/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x020924ac..0x02092654, 424 bytes, 14 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char data_020924ac[16];
extern char data_020924bc[16];
extern char data_020924cc[16];
extern char data_020924dc[16];
extern char data_020924ec[16];
extern char data_020924fc[16];
extern char data_0209250c[16];
extern char data_0209251c[16];
extern char data_0209252c[16];
extern char data_0209253c[16];
extern char data_0209254c[16];
extern char data_0209255c[20];
extern char data_02092570[20];
extern char data_0209230c[];
extern char data_02092390[];
extern char data_02092348[];
extern char data_0209233c[];
extern char data_0209242c[];
extern char data_0209247c[];
extern char data_020922f0[];
extern char data_02092438[];
extern char data_0209249c[];
extern char data_020922d8[];
extern char data_02092378[];
extern char data_020922e8[];
extern char data_02092444[];
extern char data_02092384[];
extern char data_020923c0[];
extern char data_02092354[];
extern char data_0209236c[];
extern char data_020923cc[];
extern char data_02092330[];
extern char data_02092414[];
extern char data_020923f0[];
extern char data_020923fc[];
extern char data_0209246c[];
extern char data_02092408[];
extern char data_020922e0[];
extern char data_02092300[];
extern char data_02092318[];
extern char data_020923b4[];
extern char data_02092450[];
extern char data_020923d8[];
extern char data_020923e4[];
extern char data_020923a8[];
extern char data_02092420[];
extern char data_02092324[];
extern char data_02092360[];
extern char data_0209245c[];
extern char data_0209248c[];
extern char data_0209239c[];
extern char data_020922f8[];

/* 0x0209255c: 20 bytes. */
char data_0209255c[20] = "Peach Slider Map";

/* 0x02092570: 20 bytes. */
char data_02092570[20] = "Onimasu Test Map";

/* 0x0209254c: 16 bytes. */
char data_0209254c[16] = "Battan King Map";

/* 0x0209253c: 16 bytes. */
char data_0209253c[16] = "Main Garden Map";

/* 0x0209252c: 16 bytes. */
char data_0209252c[16] = "Main Castle Map";

/* 0x0209251c: 16 bytes. */
char data_0209251c[16] = "Rainbow Cruise";

/* 0x0209250c: 16 bytes. */
char data_0209250c[16] = "Ex Wario Boss";

/* 0x020924fc: 16 bytes. */
char data_020924fc[16] = "Ex Mario Boss";

/* 0x020924bc: 16 bytes. */
char data_020924bc[16] = "Ex Mario Map";

/* 0x020924dc: 16 bytes. */
char data_020924dc[16] = "Ex Luigi Boss";

/* 0x020924cc: 16 bytes. */
char data_020924cc[16] = "Kaizoku Irie";

/* 0x020924ec: 16 bytes. */
char data_020924ec[16] = "Rainbow Mario";

/* 0x020924ac: 16 bytes. */
char data_020924ac[16] = "Metal Switch";

/* 0x02092584: 208 bytes. */
char *STAGE_DEBUG_NAMES[52] = {
    (char *)&data_0209230c, (char *)&data_0209252c, (char *)&data_02092390, (char *)&data_0209253c,
    (char *)&data_02092348, (char *)&data_0209233c, (char *)&data_0209242c, (char *)&data_0209254c,
    (char *)&data_020924cc, (char *)&data_0209247c, (char *)&data_020922f0, (char *)&data_02092438,
    (char *)&data_0209249c, (char *)&data_020922d8, (char *)&data_02092378, (char *)&data_020922e8,
    (char *)&data_02092444, (char *)&data_02092384, (char *)&data_020923c0, (char *)&data_02092354,
    (char *)&data_0209236c, (char *)&data_020923cc, (char *)&data_02092330, (char *)&data_02092414,
    (char *)&data_020923f0, (char *)&data_020923fc, (char *)&data_0209246c, (char *)&data_02092408,
    (char *)&data_0209251c, (char *)&data_0209255c, (char *)&data_020922e0, (char *)&data_02092300,
    (char *)&data_02092318, (char *)&data_020924ac, (char *)&data_020924ec, (char *)&data_020923b4,
    (char *)&data_02092450, (char *)&data_020923d8, (char *)&data_020923e4, (char *)&data_020923a8,
    (char *)&data_02092420, (char *)&data_02092570, (char *)&data_02092324, (char *)&data_02092360,
    (char *)&data_020924bc, (char *)&data_020924fc, (char *)&data_0209245c, (char *)&data_020924dc,
    (char *)&data_0209248c, (char *)&data_0209250c, (char *)&data_0209239c, (char *)&data_020922f8,
};
