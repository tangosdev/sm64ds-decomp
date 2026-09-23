//cpp
// @symbol _ZN8daDemo_c10anmModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs. */
#include "daDemo_c.h"
#include "SharedFilePtr.h"

typedef void (*VFN)(void *);

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~anmModel_c() it
 * emits, which the class's D1 file already defines. A qualified call to
 * it from here would not be the face's call: anmModel_c has a virtual
 * base, so MSVC treats an explicit destructor call as the complete-object
 * (vbase) destructor and adds the virtual base's teardown and an EH frame.
 * port/faces_sync.txt's face for this D0 called the flat D1 name, so this
 * arm makes the same two calls that face made: the D1 body through its
 * flat name, then the class-specific operator delete. Nothing here
 * reaches mwccarm: it builds the `#else` arm and emits the ROM bytes it
 * always emitted, and the object is byte-identical either way. */
extern "C" void _ZN8daDemo_c10anmModel_cD1Ev(void *self);
extern "C" daDemo_c::anmModel_c *_ZN8daDemo_c10anmModel_cD0Ev(daDemo_c::anmModel_c *thiz)
{
    _ZN8daDemo_c10anmModel_cD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    daDemo_c::anmModel_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
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
#endif
