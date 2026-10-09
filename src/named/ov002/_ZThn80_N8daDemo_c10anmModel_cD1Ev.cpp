//cpp
// @symbol _ZThn80_N8daDemo_c10anmModel_cD1Ev
/* Compiler-owned complete-destructor adjustment thunk. The source describes
   the real destructor; mwccarm emits the -0x50 thunk named by this file as a
   byproduct of the Animation base at +0x50 inside the model base.

   The nested classes are declared here under the project's spellings
   (Model, ModelAnim, Animation, ModelBase) rather than the cartridge RTTI
   spellings daDemo_c.h mirrors as dExtModel_c/dExtAnmModel_c -- the two
   spellings are the same classes with the same layout and the same vtable
   bytes (notes/model-rtti-names.md). The folded TU emits the nested vtables
   under the cartridge spellings, whose member names have no symbols.txt
   home; the copy this file emits under the project spellings resolves every
   vtable slot and is the copy romdata_check verifies. simpleModel_c's
   destructor is defined here for the same reason: no other source emits its
   vtable. */
#include "ModelAnim.h"
#include "SharedFilePtr.h"

typedef void (*VFN)(void *);

struct daDemo_c {
    struct param_c {
        Vector3 mScale[1];
    };

    struct anmModel_c : param_c, ModelAnim {
        virtual ~anmModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };

    struct simpleModel_c : param_c, Model {
        virtual ~simpleModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };
};

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

daDemo_c::simpleModel_c::~simpleModel_c()
{
    SharedFilePtr *file = *(SharedFilePtr **)((char *)this + 0x5c);
    if (file != 0) {
        file->Release();
    }
}
