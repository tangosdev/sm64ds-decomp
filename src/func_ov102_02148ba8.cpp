//cpp
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void* c);
void _ZN16MeshColliderBase6EnableEP5Actor(void* c, void* a);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);
extern unsigned char CURRENT_GAMEMODE[];

int func_ov102_02148ba8(char* c)
{
    unsigned short id = *(unsigned short*)(c+0xc);
    int r1 = 0;
    switch (id) {
    case 0x31: r1 = 0x900000; break;
    case 0x3b: r1 = 0x600000; break;
    case 0x3d: r1 = 0x600000; break;
    case 0x12a: r1 = 0x600000; break;
    case 0x154: r1 = 0x300000; break;
    case 0x155: r1 = 0x300000; break;
    }
    int on = (CURRENT_GAMEMODE[0] == 1);
    if (on) {
        if (!_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124))
            _ZN16MeshColliderBase6EnableEP5Actor((char*)c+0x124, c);
    } else {
        _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, r1, 0);
    }
    return 1;
}
}
