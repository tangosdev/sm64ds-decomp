/* Word table, 10 entries; dScMgLuigi_c indexes it as u16[20]. */
#include "types.h"

u32 sLuigiPlaceLimits[10] = {
    0x00100004, 0x00500030, 0x00300064, 0x00640050, 0x00080064,
    0x00500050, 0x00500064, 0x005a0064, 0x006e0064, 0x006e005a,
};
/* _ZTV12dScMgLuigi_c starts 8 bytes past this table; those bytes are its
   {offset-to-top, type_info} header. */
