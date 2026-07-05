//cpp
extern "C" {
void func_020393a4(void* p, int v);
int _ZN5Event6GetBitEj(unsigned int);
int _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
void func_ov029_021126dc(char* c);
int _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void*, void*, int);
int _ZN16MeshColliderBase7DisableEv(void*);

#pragma optimize_for_size on

int func_ov029_021127cc(char* c){
    func_020393a4(c+0x124, 0x100000);

    switch(*(unsigned char*)(c+0x3a2)){
    case 0:
        if(_ZN5Event6GetBitEj(*(unsigned char*)(c+0x3a4)) == 0) break;

        {
            unsigned char* st_ptr = (unsigned char*)(((int)c + 0x3a2) & 0xFFFFFFFFFFFFFFFF);
            *st_ptr = *st_ptr + 1;
        }

        *(short*)(c + 0x300 + 0xa0) = 0;
        *(unsigned char*)(c+0x3a3) = 1;

        _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
        func_ov029_021126dc(c);
        _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(c+0x124, c+0x370, *(short*)(c+0x8e));
        break;

    case 1: {
        unsigned short* p = (unsigned short*)(c + 0x3a0);
        if (*(unsigned short*)(c + 0x300 + 0xa0) > 0x168) {
            *(unsigned char*)(c + 0x3a3) = *(unsigned short*)(c + 0x300 + 0xa0) & 1;
        }
        *p = *p + 1;
        if (_ZN5Event6GetBitEj(*(unsigned char*)(c+0x3a4)) != 0) break;
        _ZN16MeshColliderBase7DisableEv(c+0x124);
        *(unsigned char*)(c+0x3a2) = 0;
        *(unsigned char*)(c+0x3a3) = 0;
        break;
    }
    }

    return 1;
}
}
