//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* act, int fix, int t, unsigned int a, unsigned int b);
extern int data_ov045_021131b0[];
extern int data_ov045_021131a8[];
extern int data_ov045_02112510[];

int _ZN15FireSeaElevator13InitResourcesEv(char* c)
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_021131b0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, -1);
    if (*(int*)(c + 8) != 0xffff) {
        int* p = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
        *p -= 0x12c000;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov045_021131a8);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, mc, c + 0x2ec, 0x199, *(short*)(c + 0x8e), data_ov045_02112510);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        c + 0x320, c, 0x35555, 0x258000, 0x280000c, 0);
    return 1;
}
}
