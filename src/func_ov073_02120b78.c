extern void _Z14ApproachLinearRsss(short* a, short b, short c);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
extern void func_ov073_0211f2c0(void* c, int a);
extern short Vec3_HorzAngle(const void* v0, const void* v1);
extern void func_02012694(int a, void* b);
extern void ChiefChilly_ChangeState(void* c, void* p);
extern int data_ov073_02123400[];

int func_ov073_02120b78(char* c){
    _Z14ApproachLinearRsss((short*)(c+0x8c), -0x4000, 0x400);
    if(*(int*)(c+0x3dc) > *(int*)(c+0x60)){
        if(_ZNK12WithMeshClsn10IsOnGroundEv(c+0x150)){
            func_ov073_0211f2c0(c, 0xfa0000);
            *(int*)(c+0x98) = 0;
            *(short*)(c+0x94) = Vec3_HorzAngle(c+0x5c, c+0x3d8);
            *(short*)(c+0x8e) = *(short*)(c+0x94);
            func_02012694(0x16c, c+0x74);
            ChiefChilly_ChangeState(c, data_ov073_02123400);
        }
    }
    return 1;
}
