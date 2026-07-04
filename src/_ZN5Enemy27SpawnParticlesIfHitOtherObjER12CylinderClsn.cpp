//cpp
extern "C" {
extern void* _ZN5Actor10FindWithIDEj(unsigned int);
extern void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void* self, void* a, char* p);
int _ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(char* self, char* clsn) {
    int* f;
    if (*(unsigned char*)(self+0x107) != 0) {
        unsigned int id = *(unsigned int*)(clsn+0x24);
        if (id != 0) {
            void* a = _ZN5Actor10FindWithIDEj(id);
            if (a != 0) {
                unsigned short t = *(unsigned short*)((char*)a+0xc);
                int e1 = (t == 0x120);
                if (e1 == 0) {
                    int e2 = (t == 0x121);
                    if (e2 == 0) {
                        _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(self, a, clsn);
                        return 1;
                    }
                }
            }
        }
        f = (int*)(((int)clsn + 0x18) & 0xFFFFFFFFFFFFFFFF);
        *f = *f | 0x20000;
        goto done;
    }
    f = (int*)(((int)clsn + 0x18) & 0xFFFFFFFFFFFFFFFF);
    *f = *f & ~0x20000;
done:
    return 0;
}
}
