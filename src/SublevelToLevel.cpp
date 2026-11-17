//cpp
/* Sublevel id to course id. SUBLEVEL_LEVEL_TABLE is the lookup.
 * sSublevelInit exists so this file emits the empty initializer at
 * 0x02074dc0 (bx lr, no relocations), between GetLevelPart.cpp and
 * Timer.cpp. The object is deadstripped: the initializer never references
 * it, and a .bss byte in this gap would shift every later symbol.
 */
extern const signed char SUBLEVEL_LEVEL_TABLE[];

extern "C" int SublevelToLevel(int i) {
    return SUBLEVEL_LEVEL_TABLE[i];
}

struct SublevelInit {
    SublevelInit() {}
};
static SublevelInit sSublevelInit;

extern const signed char SUBLEVEL_LEVEL_TABLE[52] = {
    -1, 29, 29, 29, 29, 29, 0,  1,  2,  2,  3,  3,  4,  5,  6,  6,  7,  7,
    8,  9,  9,  10, 11, 11, 12, 12, 12, 13, 14, 21, 22, 23, 24, 25, 26, 15,
    15, 16, 16, 17, 17, -1, 28, 27, 18, 18, 19, 19, 20, 20, 29, 29,
};
