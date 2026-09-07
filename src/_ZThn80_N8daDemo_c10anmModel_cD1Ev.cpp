//cpp
// @symbol _ZThn80_N8daDemo_c10anmModel_cD1Ev
/* Compiler-owned complete-destructor adjustment thunk. The source describes
   the real destructor; mwccarm emits the -0x50 thunk named by this file as a
   byproduct of the virtual ScaleHolder base in daDemo_c.h. */
#include "daDemo_c.h"
#include "SharedFilePtr.h"

typedef void (*VFN)(void *);

daDemo_c::anmModel_c::~anmModel_c()
{
    char *c = (char *)this;
    void *p;
    int i;

    p = *(void **)(c + 0x70);
    if (p != 0) ((SharedFilePtr *)(p))->Release();
    for (i = 0; i < *(unsigned char *)(c + 0x80); i++) {
        p = (*(void ***)(c + 0x74))[i];
        if (p != 0) ((SharedFilePtr *)(p))->Release();
    }
    if (*(void **)(c + 0x7c) != 0) {
        for (i = 0; i < *(unsigned char *)(c + 0x81); i++) {
            p = (*(void ***)(c + 0x78))[i];
            if (p != 0) ((SharedFilePtr *)(p))->Release();
        }
        p = *(void **)(c + 0x7c);
        if (p != 0) {
            (*(VFN)((*(int **)p)[1]))(p);
        }
    }
}
