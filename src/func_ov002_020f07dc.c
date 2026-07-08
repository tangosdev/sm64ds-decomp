extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, int radius, int height, unsigned int flags, unsigned int vulnFlags);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int RED_NUMBER_MODEL_PTR;
int func_ov002_020f07dc(char* c){
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0xd4, c, 0x64000, 0x40000, 0x800002, 0);
    *(unsigned char*)(c + 0x10d) = *(unsigned int*)(c + 8) & 0xf;
    *(unsigned char*)(c + 0x10e) = (*(unsigned int*)(c + 8) >> 8) & 0xf;
    *(unsigned char*)(c + 0x10f) = 0;
    *(int*)(c + 0x108) = 0;
    *(unsigned char*)(c + 0x110) = 0;
    *(unsigned char*)(c + 0x113) = 0;
    _ZN5Model8LoadFileER13SharedFilePtr(&RED_NUMBER_MODEL_PTR);
    return 1;
}
