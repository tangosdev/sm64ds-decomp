/* Pointers, 14 entries. */
#include "types.h"

extern struct OamAttr data_ov006_0213e688[];
extern struct OamAttr data_ov006_0213e848[];
extern struct OamAttr data_ov006_0213e888[];
extern struct OamAttr data_ov006_0213e8a8[];
extern struct OamAttr data_ov006_0213e908[];
extern struct OamAttr data_ov006_0213e928[];
extern u32 data_ov006_0213ea60[];
extern struct OamAttr sSlot1GameOverReelSprite;
extern struct OamAttr sSlot1SymbolIcon0;
extern struct OamAttr sSlot1SymbolIcon1;
extern struct OamAttr sSlot1SymbolIcon2;
extern struct OamAttr sSlot1SymbolIcon3;
extern struct OamAttr sSlot1SymbolIcon4;
extern struct OamAttr sSlot1SymbolIcon5;

void *sSlot1Sprites[14] = {
    (void *)&data_ov006_0213e848, (void *)&data_ov006_0213e688,      (void *)&data_ov006_0213e928,
    (void *)&data_ov006_0213e888, (void *)&data_ov006_0213e8a8,      (void *)&data_ov006_0213ea60,
    (void *)&sSlot1SymbolIcon0,   (void *)&sSlot1SymbolIcon1,        (void *)&sSlot1SymbolIcon2,
    (void *)&sSlot1SymbolIcon3,   (void *)&sSlot1SymbolIcon4,        (void *)&sSlot1SymbolIcon5,
    (void *)&data_ov006_0213e908, (void *)&sSlot1GameOverReelSprite,
};
