//cpp
// @symbol func_ov002_020bd480
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

namespace Sound{ void PlayCharVoice(unsigned int, unsigned int, const Vector3 &); }
extern "C" int func_ov002_020bd480(unsigned char *c, unsigned char *p){
    unsigned int v=ReadUnalignedInt(p);
    Sound::PlayCharVoice(c[0x6d9], v, *(Vector3*)(c+0x74));
    return 1;
}
