extern short Vec3_HorzAngle(const void* a, const void* b);
extern int _ZN13MontyMoleRockD0Ev(void* t);
extern char* _ZN5Actor13ClosestPlayerEv(void* c);
extern int Vec3_Dist(const void* a, const void* b);
extern int func_ov078_02125c48(void* c, void* p);
extern void func_02012694(int a, void* b);
extern void ApproachAngle(short* a, short b, short c, short d, int e);
extern int data_ov078_0212703c[];
extern int data_ov078_0212710c[];

struct Vec3 { int x,y,z; };

int func_ov078_02123aa0(char* c){
    short ang = Vec3_HorzAngle(c+0x5c, c+0x4e0);
    if(_ZN13MontyMoleRockD0Ev(c) == 1) return 1;
    char* p = _ZN5Actor13ClosestPlayerEv(c);
    if(p != 0){
        struct Vec3 v = *(struct Vec3*)(p+0x5c);
        if(Vec3_Dist(c+0x4d4, &v) < 0x640000){
            if(*(int*)(c+0x4d8) - 0x64000 < v.y){
                func_ov078_02125c48(c, data_ov078_0212703c);
                return 1;
            }
        }
    }
    if(*(unsigned char*)(c+0x505) == 0){
        func_02012694(0x12d, c+0x74);
        *(unsigned char*)(c+0x505) = 5;
    }
    ApproachAngle((short*)(c+0x94), ang, 5, 0x1000, 0x100);
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    if(Vec3_Dist(c+0x5c, c+0x4e0) < 0x32000){
        func_ov078_02125c48(c, data_ov078_0212710c);
    }
    return 1;
}
