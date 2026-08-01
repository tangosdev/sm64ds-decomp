// @symbol func_ov064_02119ea0
// recovered name: BowserPuzzlePiece_Kill
/* recovered: renamed to Class_Method */
/* daWater_Hakidasi_c::Kill - recovered from vtable slot identity */
typedef signed short s16;
typedef int s32;
extern s32 _ZN5Actor18HorzAngleToCPlayerEv(void* self);
s32 func_ov064_02119ea0(char* c) {
    *(s16*)(c + 0x100) = 0xc8;
    s32 angle = _ZN5Actor18HorzAngleToCPlayerEv(c);
    *(s16*)(c + 0x388) = (s16)angle;
    return 1;
}
