//cpp
#include "BMD_File.h"

extern "C" {
extern void *func_ov007_020c162c(void *, int, int, int, int, unsigned short);

// Keep the existing address-named caller ABI while decoding the model file
// through its established texture and palette records.
void *func_ov007_020b2308(void *fileData, int index) {
    BMD_File *file = static_cast<BMD_File *>(fileData);
    BMD_Texture *texture = &file->textures[index];
    unsigned flags = texture->flags;
    BMD_Palette *palette = &file->palettes[index];
    return func_ov007_020c162c(reinterpret_cast<void *>((flags << 3) >> 29),
                               reinterpret_cast<int>(texture->data), (flags << 9) >> 29,
                               (flags << 6) >> 29, reinterpret_cast<int>(palette->data),
                               static_cast<unsigned short>(static_cast<int>(palette->size) / 2));
}
}
