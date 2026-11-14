//cpp
/* Sublevel id to course id. SUBLEVEL_LEVEL_TABLE is the lookup.
 * sSublevelInit exists so this file emits the empty initializer at
 * 0x02074dc0 (bx lr, no relocations), between GetLevelPart.cpp and
 * Timer.cpp. The object is deadstripped: the initializer never references
 * it, and a .bss byte in this gap would shift every later symbol.
 */
extern signed char SUBLEVEL_LEVEL_TABLE[];

extern "C" int SublevelToLevel(int i) {
    return SUBLEVEL_LEVEL_TABLE[i];
}

struct SublevelInit {
    SublevelInit() {}
};
static SublevelInit sSublevelInit;
