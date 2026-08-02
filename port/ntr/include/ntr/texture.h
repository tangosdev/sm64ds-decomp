// DS texture formats -> RGBA.
#ifndef NTR_TEXTURE_H
#define NTR_TEXTURE_H

#include <stdint.h>
#include <vector>

namespace ntr {

struct TextureDesc {
    const uint8_t *data = nullptr;   int32_t data_len = 0;
    const uint8_t *index = nullptr;  int32_t index_len = 0;   // format 5 only
    const uint8_t *pal = nullptr;    int32_t pal_len = 0;
    int width = 0, height = 0;
    int format = 0;                  // TEXIMAGE_PARAM bits 26..28
    bool color0_transparent = false;
};

// Decode into `out` (width*height, 0xAARRGGBB). False if the format is
// unsupported or the buffers are too short for the stated dimensions.
bool texture_decode(const TextureDesc &d, std::vector<uint32_t> &out);

}  // namespace ntr

#endif
