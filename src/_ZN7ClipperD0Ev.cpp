//cpp
// @symbol _ZN7ClipperD0Ev
/* D0, the deleting destructor. Same shadow as the D1 file beside this one; one
 * destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. */
#include "types.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

/* A shadow with the members spelled as raw padding, on purpose. Clipper's real
 * header gives it Vector3 members, and include/types.h declares ~Vector3() {}
 * because the ROM's __destroy_arr calls prove the element type is non-POD -- so
 * the real header makes mwcc emit member teardown the cartridge's destructor
 * does not have. The ROM's Clipper::~Clipper is a single vptr store and nothing
 * else, which is what padding reproduces. */
struct Clipper {
    virtual ~Clipper();
    /* Clipper deallocates through Memory::operator_delete2 (0x0203cbcc), not the
     * actor heap and not the global _ZdlPv. CW inlines operator delete into the
     * deleting destructor only when it finds one on the class or its immediate
     * base, so it has to live here. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
};

Clipper::~Clipper()
{
}
