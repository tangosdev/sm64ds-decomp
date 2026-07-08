extern int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj();
extern int _ZN5Model8LoadFileER13SharedFilePtr();
extern int SUPER_MUSHROOM_MODEL_PTR[];
int func_ov002_020b49a8(char* c){
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj((char*)c+0xd4, c, 0x12c000, 0x32000, 0x800002, 0x20);
    *(char*)(c+0x109) = *(int*)(c+8);
    _ZN5Model8LoadFileER13SharedFilePtr(SUPER_MUSHROOM_MODEL_PTR);
    return 1;
}
