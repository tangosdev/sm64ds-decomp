/* Pointers, 3 entries. */
#include "types.h"

extern struct OamAttr _ZN3OAM15TOUCH_MODE_TEXTE[];
extern struct OamAttr _ZN3OAM18STANDARD_MODE_TEXTE[];
extern struct OamAttr _ZN3OAM19DUAL_HAND_MODE_TEXTE[];

struct OamAttr *_ZN3OAM21CONTROLLER_MODE_TEXTSE[3] = {
    (struct OamAttr *)&_ZN3OAM18STANDARD_MODE_TEXTE,
    (struct OamAttr *)&_ZN3OAM15TOUCH_MODE_TEXTE,
    (struct OamAttr *)&_ZN3OAM19DUAL_HAND_MODE_TEXTE,
};
