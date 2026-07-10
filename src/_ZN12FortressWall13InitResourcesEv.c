typedef int Fix12;
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mtx, Fix12 f, short s, void *blk);
struct E12 { void *p; int pad[2]; };
extern struct E12 data_ov079_02128058[];
extern struct E12 data_ov079_0212805c[];
extern struct E12 data_ov079_02128060[];
int _ZN12FortressWall13InitResourcesEv(char *c) {
    int b = (int)(*(unsigned short*)(c + 0xc) == 0x30);
    if (b != 0) {
        *(unsigned char*)(c + 0x31e) = 0;
        *(unsigned char*)(c + 0x31f) = (unsigned char)*(int*)(c + 8);
        if (*(unsigned char*)(c + 0x31f) == 0xff)
            *(unsigned char*)(c + 0x31f) = 0;
    } else {
        *(unsigned char*)(c + 0x31e) = 1;
    }
    {
        int idx = *(unsigned char*)(c + 0x31e);
        void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_02128058[idx].p);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, mdl, 1, -1);
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
        {
            int idx2 = *(unsigned char*)(c + 0x31e);
            void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov079_0212805c[idx2].p);
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, kcl, c + 0x2ec, 0x199, *(short*)(c + 0x8e), data_ov079_02128060[idx2].p);
        }
    }
    return 1;
}
