typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Vector3 { int x, y, z; } Vector3;

enum Bool { FALSE, TRUE };

extern Vector3 data_ov100_02148008;

extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* clsn, const Vector3* pos);
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* pos, u32 a, int b, u32 c, u32 d, u32 e);

void func_ov100_0214344c(char* self)
{
    Vector3 v;
    char* other;
    
    int flags;
    u32 id;

    v.x = data_ov100_02148008.x;
    v.y = data_ov100_02148008.y;
    v.z = data_ov100_02148008.z;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(self + 0x110, &v);
    id = *(u32*)(self + 0x134);
    if (id == 0) return;
    other = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (other == 0) return;
    flags = *(int*)(self + 0x130);
    if (flags & 0x4000) {
        *(u16*)(self + 0x6a6) = 0x5a;
        *(u32*)(self + 0x80) = 0x2000;
        *(u32*)(self + 0x84) = *(u32*)(self + 0x80);
        *(u32*)(self + 0x88) = *(u32*)(self + 0x84);
        return;
    }
    if (flags & 0x10) {
        *(u16*)(self + 0x6a6) = 0x5a;
        *(u32*)(self + 0x80) = 0x2000;
        *(u32*)(self + 0x84) = *(u32*)(self + 0x80);
        *(u32*)(self + 0x88) = *(u32*)(self + 0x84);
        return;
    }
    
    {
        enum Bool b = (enum Bool)(*(u16*)(other + 0xc) == 9);
        if (b != FALSE && (flags & 0x2000)) {
            *(u16*)(self + 0x6a6) = 0x5a;
            *(u32*)(self + 0x80) = 0x2000;
            *(u32*)(self + 0x84) = *(u32*)(self + 0x80);
            *(u32*)(self + 0x88) = *(u32*)(self + 0x84);
            return;
        }
    }
    {
        enum Bool b = (enum Bool)(*(u16*)(other + 0xc) == 0xbf);
        if (b == FALSE) return;
    }
    if (*(u8*)(other + 0x6fb) != 0) return;
    {
        Vector3 pos;
        pos.x = *(int*)(self + 0x5c);
        pos.y = *(int*)(self + 0x60);
        pos.z = *(int*)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &pos, 2, 0xc000, 1, 0, 1);
    }
}
