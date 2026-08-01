#include "types.h"
// @symbol _ZN5Model17LoadTextureToVramEPcj
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Model.h"
u32 _ZN5Model17LoadTextureToVramEPcj(char* texData, u32 size) {
    u32 vramOffset = _ZN5Model13GetVramOffsetEj(size);
    _ZN2GX12BeginLoadTexEv();
    _ZN2GX7LoadTexEPKvjj((const void*)texData, vramOffset, size);
    data_020a4be4 += size;
    _ZN2GX10EndLoadTexEv();
    return vramOffset;
}
