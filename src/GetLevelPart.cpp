//cpp
/* Which part of a course a sublevel belongs to. data_02075264 is that table.
 * sLevelPartInit exists so this file emits the empty initializer at
 * 0x02074dbc (bx lr, no relocations), between Sound.cpp and
 * SublevelToLevel.cpp. The object is deadstripped: the initializer never
 * references it, and a .bss byte in this gap would shift every later symbol.
 */
extern signed char data_02075264[];

extern "C" signed char GetLevelPart(int idx) {
    return data_02075264[idx];
}

struct LevelPartInit {
    LevelPartInit() {}
};
static LevelPartInit sLevelPartInit;
