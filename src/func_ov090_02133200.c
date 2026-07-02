typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern s32 Vec3_Dist(void* a, void* b);
extern s16 Vec3_HorzAngle(void* a, void* b);
extern int ApproachAngle(s16* angle, int target, int invFactor, int maxDelta, int minDelta);
extern int func_ov090_021332e8(void* c, void* p);

extern int data_ov090_02134584[];

int func_ov090_02133200(char* c)
{
    if (Vec3_Dist(c + 0x5c, c + 0x374) > 0x3e8000) {
        *(u16*)(c + 0x100) = 0x32;
        *(s16*)(c + 0x384) = Vec3_HorzAngle(c + 0x5c, c + 0x374);
    }
    ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x384), 1, 0x100, 0x200);
    if (*(u16*)(c + 0x100) == 0)
        func_ov090_021332e8(c, data_ov090_02134584);
    return 1;
}
