typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, Fix12, short, void*);
extern int data_ov018_02113c84;
extern int data_ov018_02113c7c;
extern int data_ov002_0210d754;

int _ZN8IceSheet13InitResourcesEv(char *c)
{
    void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov018_02113c84);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, mdl, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov018_02113c7c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, kcl, c + 0x2ec, 0x199, *(short *)(c + 0x8e), (void *)&data_ov002_0210d754);
    return 1;
}
