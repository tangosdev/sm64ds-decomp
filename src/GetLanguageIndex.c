#include "types.h"

extern s32 _Z16GetOwnerLanguagev(void);

/* The console language as an index into the per-language asset tables:
   English 0, French 1, German 2, Italian 3, Spanish 4; anything else 0. */
s32 GetLanguageIndex(void) {
    s32 lang = _Z16GetOwnerLanguagev();
    switch (lang) {
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    default:
        return 0;
    }
}
