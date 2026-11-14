#include "types.h"
#include "BMD_File.h"
#include "Ov007Texture.h"

/* Point the menu texture at the texel and palette data of record `i` in a
 * loaded BMD model file. */
void func_ov007_020b22e0(Ov007Texture *tex, struct BMD_File *file, int i) {
    struct BMD_Texture *texture = &file->textures[i];
    struct BMD_Palette *palette = &file->palettes[i];

    tex->texSrc = texture->data;
    tex->plttSrc = palette->data;
}
