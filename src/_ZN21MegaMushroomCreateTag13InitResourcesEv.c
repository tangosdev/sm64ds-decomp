extern int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj();
extern int _ZN5Model8LoadFileER13SharedFilePtr();
extern int data_ov002_0210da30[];
int _ZN21MegaMushroomCreateTag13InitResourcesEv(char* c){
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj((char*)c+0xd4, c, 0x12c000, 0x32000, 0x800002, 0x20);
    *(char*)(c+0x109) = *(int*)(c+8);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da30);
    return 1;
}
