//cpp
/* .rodata data at 0x02114824; bytes from the cartridge. */
#include "types.h"

struct MkyAnimFilePtr;

extern unsigned int
    gMonkeyHoldAnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyJumpStAnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyJumpEdAnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyRunStAnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyRunAnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyRunEdAnimationFile[]; /* local extern: linker-name reference for a data word */
extern struct MkyAnimFilePtr
    data_ov030_02115cf0; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyWait2AnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyWait3AnimationFile[]; /* local extern: linker-name reference for a data word */
extern unsigned int
    gMonkeyWait4AnimationFile[]; /* local extern: linker-name reference for a data word */

extern struct SharedFilePtr *const data_ov030_02114824[10] = {
    (struct SharedFilePtr *)gMonkeyHoldAnimationFile,
    (struct SharedFilePtr *)gMonkeyJumpStAnimationFile,
    (struct SharedFilePtr *)gMonkeyJumpEdAnimationFile,
    (struct SharedFilePtr *)gMonkeyRunStAnimationFile,
    (struct SharedFilePtr *)gMonkeyRunAnimationFile,
    (struct SharedFilePtr *)gMonkeyRunEdAnimationFile,
    (struct SharedFilePtr *)&data_ov030_02115cf0,
    (struct SharedFilePtr *)gMonkeyWait2AnimationFile,
    (struct SharedFilePtr *)gMonkeyWait3AnimationFile,
    (struct SharedFilePtr *)gMonkeyWait4AnimationFile};
