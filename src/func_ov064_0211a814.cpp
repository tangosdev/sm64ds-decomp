//cpp
struct SharedFilePtr { int x; };
struct BMD_File;
struct BCA_File;
struct Actor;
extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File*, int, int);
BCA_File* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, BCA_File*, int, int fix, unsigned int);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, Actor*, int fix, int t, unsigned int, unsigned int);
void func_ov064_0211a284(char* t);
unsigned char _ZN5Actor9TrackStarEjj(void* thiz, unsigned int, unsigned int);
}
extern SharedFilePtr RED_NUMBER_MODEL_PTR;
extern SharedFilePtr BUBBLE_MODEL_PTR;
extern SharedFilePtr data_ov064_0211c96c;
extern SharedFilePtr data_ov064_0211c964;

extern "C" int func_ov064_0211a814(char* c) {
    _ZN5Model8LoadFileER13SharedFilePtr(RED_NUMBER_MODEL_PTR);
    _ZN5Model8LoadFileER13SharedFilePtr(BUBBLE_MODEL_PTR);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov064_0211c96c), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4,
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov064_0211c964), 0x40000000, 0x1000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x138, (Actor*)c, 0x96000, 0x96000, 0x200004, 0);
    func_ov064_0211a284(c);
    *(unsigned char*)(c + 0x172) = *(int*)(c + 8);
    *(unsigned char*)(c + 0x174) = (unsigned int)*(int*)(c + 8) >> 8;
    if (*(unsigned char*)(c + 0x174) != 0xff) {
        *(unsigned char*)(c + 0x175) = _ZN5Actor9TrackStarEjj(c, *(unsigned char*)(c + 0x174), 2);
    }
    return 1;
}
