//cpp
// @symbol func_ov002_020bd45c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

namespace Sound{ void PlayBank0(unsigned int, const Vector3 &); }
extern "C" int func_ov002_020bd45c(unsigned char *c, unsigned char *p){
    unsigned int v=ReadUnalignedInt(p);
    Sound::PlayBank0(v, *(Vector3*)(c+0x74));
    return 1;
}
