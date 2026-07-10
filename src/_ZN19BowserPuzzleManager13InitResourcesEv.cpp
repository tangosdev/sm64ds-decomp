//cpp
struct SharedFilePtr { int x; };
struct BMD_File;
struct KCL_File;
struct Matrix4x3 { int m[12]; };
struct CLPS_Block { int x; };
extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File*, int, int);
void func_ov064_02119010(char* c);
void func_ov064_02118fa4(void* c);
KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, KCL_File*, const Matrix4x3&, int fix, short s, CLPS_Block&);
void func_020393c4(int* p, int v);
}
extern SharedFilePtr* data_ov064_0211adc8[];
extern SharedFilePtr data_ov064_0211c800;
extern CLPS_Block data_ov064_0211baac;
extern void func_ov064_021192bc();
extern int data_ov064_0211c198[];

extern "C" void _ZN19BowserPuzzleManager13InitResourcesEv(char* c) {
    *(unsigned char*)(c + 0x337) = *(int*)(c + 8) & 0xf;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(*data_ov064_0211adc8[*(unsigned char*)(c + 0x337)]), 1, -1);
    func_ov064_02119010(c);
    func_ov064_02118fa4(c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov064_0211c800),
        *(const Matrix4x3*)(c + 0x2ec), 0x1000, *(short*)(c + 0x8e), data_ov064_0211baac);
    func_020393c4((int*)(c + 0x124), (int)&func_ov064_021192bc);
    *(int*)(c + 0x324) = data_ov064_0211c198[*(unsigned char*)(c + 0x337)];
    *(unsigned char*)(c + 0x328) = 0;
    *(int*)(c + 0x32c) = 0;
    *(short*)(c + 0x334) = 0;
    *(unsigned char*)(c + 0x338) = 0;
    *(unsigned char*)(c + 0x339) = 1;
    *(unsigned char*)(c + 0x336) = 0;
    *(unsigned char*)(c + 0x33a) = 1;
}
