//cpp
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, int file, int anim, int speed, unsigned int u);
int Vec3_Dist(const void* a, const void* b);
void _ZN5Actor13SpawnSoundObjEj(void* self, unsigned int id);
}
extern int data_ov030_02115ce0[];

extern "C" int func_ov030_0211360c(char* c){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, data_ov030_02115ce0[1], 0, 0x1000, 0);
    *(int*)(c+0x130) = 0x1000;
    if (Vec3_Dist(c+0x380, c+0x5c) < 0x514000
        && *(int*)(c+0x60) > *(int*)(c+0x384) - 0x12c000) {
        *(unsigned char*)(c+0x3c7) = 0;
        _ZN5Actor13SpawnSoundObjEj(c, 1);
    } else {
        *(unsigned char*)(c+0x3c7) = 2;
    }
    *(unsigned char*)(c+0x3c6) = 0x3c;
    *(int*)(c+0x3b4) = 4;
    return 1;
}
