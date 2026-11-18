/* The nine NitroSDK FS command handlers, indexed by FS_COMMAND
   (FSi_TranslateCommand falls back to this table). */
#include "types.h"

struct FSFile;

extern int (*const fsi_default_command[9])(struct FSFile *p_file) = {
    (int (*)(struct FSFile *))0x0205c448, (int (*)(struct FSFile *))0x0205c410,
    (int (*)(struct FSFile *))0x0205c378, (int (*)(struct FSFile *))0x0205c264,
    (int (*)(struct FSFile *))0x0205c048, (int (*)(struct FSFile *))0x0205bc88,
    (int (*)(struct FSFile *))0x0205bc0c, (int (*)(struct FSFile *))0x0205bbe4,
    (int (*)(struct FSFile *))0x0205bbdc,
};
