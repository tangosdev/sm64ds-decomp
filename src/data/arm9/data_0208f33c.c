/* Message font and glyph tables.
 * MSG_GEN_TEXT_FUNCS is the three-entry text-expansion vtable; the 32-byte
 * records are the per-width glyph strips and 0x0208eee0..0x0208f074 the
 * u16 (code, glyph) map Message2/Message3 index.
 * Consumer: src/Message.cpp, Message2.cpp, Message3.cpp.
 * ROM 0x0208f33c..0x0208f3a4, 104 bytes, 8 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTIN5dPa_c7level_c16simpleCallback_cE[3];
extern u8 _ZTVN3abi20__si_class_type_infoE[];
extern char _ZTSN5dPa_c7level_c20edStarKiraCallback_cE[];
extern char _ZTSN5dPa_c7level_c16splashCallback_cE[];
extern char _ZTSN5dPa_c7level_c15scaleCallback_cE[];
extern char _ZTSN5dPa_c7level_c16simpleCallback_cE[];
extern u32 _ZTIN5dPa_c7level_c10callback_cE[];
extern char _ZTSN5dPa_c7level_c14clipCallback_cE[];
extern char _ZTSN5dPa_c7level_c24fitWaterSimpleCallback_cE[];
extern char _ZTSN5dPa_c7level_c23cleanParticleCallback_cE[];
extern char _ZTSN5dPa_c7level_c18fitWaterCallback_cE[];
extern u32 _ZTIN5dPa_c7level_c16bubbleCallback_cE[];

/* 0x0208f384: 12 bytes. */
u32 _ZTIN5dPa_c7level_c23cleanParticleCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c23cleanParticleCallback_cE,
    (u32)_ZTIN5dPa_c7level_c10callback_cE,
};

/* 0x0208f378: 12 bytes. */
u32 _ZTIN5dPa_c7level_c24fitWaterSimpleCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c24fitWaterSimpleCallback_cE,
    (u32)_ZTIN5dPa_c7level_c16simpleCallback_cE,
};

/* 0x0208f36c: 12 bytes. */
u32 _ZTIN5dPa_c7level_c14clipCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c14clipCallback_cE,
    (u32)_ZTIN5dPa_c7level_c10callback_cE,
};

/* 0x0208f360: 12 bytes. */
u32 _ZTIN5dPa_c7level_c16simpleCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c16simpleCallback_cE,
    (u32)_ZTIN5dPa_c7level_c10callback_cE,
};

/* 0x0208f354: 12 bytes. */
u32 _ZTIN5dPa_c7level_c15scaleCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c15scaleCallback_cE,
    (u32)_ZTIN5dPa_c7level_c16simpleCallback_cE,
};

/* 0x0208f348: 12 bytes. */
u32 _ZTIN5dPa_c7level_c16splashCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c16splashCallback_cE,
    (u32)_ZTIN5dPa_c7level_c16simpleCallback_cE,
};

/* 0x0208f33c: 12 bytes. */
u32 _ZTIN5dPa_c7level_c20edStarKiraCallback_cE[3] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c20edStarKiraCallback_cE,
    (u32)_ZTIN5dPa_c7level_c16simpleCallback_cE,
};

/* 0x0208f390: 20 bytes. */
u32 _ZTIN5dPa_c7level_c18fitWaterCallback_cE[5] = {
    (u32)_ZTVN3abi20__si_class_type_infoE,       (u32)_ZTSN5dPa_c7level_c18fitWaterCallback_cE,
    (u32)_ZTIN5dPa_c7level_c10callback_cE,       0x00000000,
    (u32)_ZTIN5dPa_c7level_c16bubbleCallback_cE,
};
