//cpp
/* .data data at 0x02115bc8; bytes from the cartridge. */
#include "types.h"

struct MkyAnimFilePtr;

extern struct MkyAnimFilePtr
    data_ov030_02115cf0; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyWait4AnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyWait2AnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyWait3AnimationFile[];    /* local extern: linker-name reference for a data word */
extern unsigned int _ZTI7daMky_c[]; /* local extern: linker-name reference for a data word */

unsigned int data_ov030_02115bc8[13] = {(u32)&data_ov030_02115cf0,
                                        (u32)gMonkeyWait4AnimationFile,
                                        (u32)&data_ov030_02115cf0,
                                        (u32)gMonkeyWait2AnimationFile,
                                        (u32)gMonkeyWait2AnimationFile,
                                        (u32)gMonkeyWait2AnimationFile,
                                        (u32)gMonkeyWait2AnimationFile,
                                        (u32)gMonkeyWait3AnimationFile,
                                        (u32)gMonkeyWait4AnimationFile,
                                        (u32)gMonkeyWait3AnimationFile,
                                        (u32)gMonkeyWait4AnimationFile,
                                        0x00000000,
                                        (u32)_ZTI7daMky_c};
