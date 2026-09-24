//cpp
// @symbol _ZN8daDemo_c13simpleModel_cD0Ev
/* D0, the deleting destructor. Same class shape as the D1 file beside this one;
 * one destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. The class operator delete is what routes the tail
 * call to Memory::operator_delete2 (0x0203cbcc) rather than the global _ZdlPv --
 * without it the bytes still match and only the relocation destination differs. */
#include "daDemo_c.h"
#include "SharedFilePtr.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. MSVC
 * folds the Itanium destructor variants into the one ~simpleModel_c() it
 * emits, which the class's D1 file already defines. A qualified call to
 * it from here would not be the face's call: simpleModel_c has a virtual
 * base, so MSVC treats an explicit destructor call as the complete-object
 * (vbase) destructor and adds the virtual base's teardown and an EH frame.
 * port/faces_sync.txt's face for this D0 called the flat D1 name, so this
 * arm makes the same two calls that face made: the D1 body through its
 * flat name, then the class-specific operator delete. Nothing here
 * reaches mwccarm: it builds the `#else` arm and emits the ROM bytes it
 * always emitted, and the object is byte-identical either way. */
extern "C" void _ZN8daDemo_c13simpleModel_cD1Ev(void *self);
extern "C" daDemo_c::simpleModel_c *_ZN8daDemo_c13simpleModel_cD0Ev(daDemo_c::simpleModel_c *thiz)
{
    _ZN8daDemo_c13simpleModel_cD1Ev(thiz);           /* the D1 body, through its flat ROM name */
    daDemo_c::simpleModel_c::operator delete(thiz);  /* the class-specific delete D0 ends with */
    return thiz;
}
#else
daDemo_c::simpleModel_c::~simpleModel_c()
{
    SharedFilePtr *file = *(SharedFilePtr **)((char *)this + 0x5c);
    if (file != 0) {
        file->Release();
    }
}
#endif
