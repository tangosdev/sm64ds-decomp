void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* clsn);
void func_020383fc(void* p);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
int _ZN5Actor13DistToCPlayerEv(void* p);
void _ZN5Actor14TriplePoofDustEv(void* p);
int _ZN16MeshColliderBase9IsEnabledEv(void* p);
void _ZN16MeshColliderBase7DisableEv(void* p);
void _ZN8Platform21UpdateModelPosAndRotYEv(void* p);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* p, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* p);

int _ZN8ShipWing8BehaviorEv(char* c) {
    switch (*(unsigned char*)(c + 0x4ea)) {
    case 0:
        if (*(unsigned char*)(c + 0x4e8) == 0) {
            *(unsigned char*)(c + 0x4e9) = 0;
        } else {
            unsigned char* pc9 = (unsigned char*)(((int)c + 0x4e9) & 0xFFFFFFFFFFFFFFFF);
            *pc9 = *pc9 + 1;
            *(unsigned char*)(c + 0x4e8) = 0;
        }
        if (*(unsigned char*)(c + 0x4e9) >= 0xf) *(unsigned char*)(c + 0x4ea) = 1;
        break;
    case 1:
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        func_020383fc(c + 0x320);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320) == 0) {
            if (_ZN5Actor13DistToCPlayerEv(c) <= 0x9c4000) break;
        }
        _ZN5Actor14TriplePoofDustEv(c);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) != 0) _ZN16MeshColliderBase7DisableEv(c + 0x124);
        *(int*)(c + 0x5c) = *(int*)(c + 0x4dc);
        *(int*)(c + 0x60) = *(int*)(c + 0x4e0);
        *(int*)(c + 0x64) = *(int*)(c + 0x4e4);
        *(unsigned char*)(c + 0x4ea) = 2;
        break;
    case 2: {
        int d = _ZN5Actor13DistToCPlayerEv(c);
        if (d <= 0x3e8000) break;
        if (d < 0x7d0000) {
            *(int*)(c + 0xa8) = 0;
            *(unsigned char*)(c + 0x4e9) = 0;
            *(unsigned char*)(c + 0x4e8) = 0;
            *(unsigned char*)(c + 0x4ea) = 0;
        }
        break;
    }
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (*(unsigned char*)(c + 0x4ea) != 2) {
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x5dc000, 0) != 0) {
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        }
    }
    return 1;
}
