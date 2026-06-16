typedef short s16;
extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
s16 func_ov062_02116850(void* c) {
    *(int*)((char*)c + 0xa8) = 0;
    s16 angle = Vec3_HorzAngle((char*)c + 0x5c, (char*)c + 0x3c0);
    *(s16*)((char*)c + 0x3f4) = angle;
    *(s16*)((char*)c + 0x100) = 0x14;
    *(int*)((char*)c + 0x98) = 0;
    return 1;
}
