//cpp
struct Vector3 { int x, y, z; };
struct Vector3_16 { unsigned short x, y, z; };
struct ActorBase { void MarkForDestruction(); };
struct Actor {
    static Actor* Spawn(unsigned int a, unsigned int b, const Vector3& v, const Vector3_16* v16, int e, int f);
};

extern "C" void func_ov081_02127be0(char* c)
{
    char* o = *(char**)(c + 0x364);
    if (o == 0) return;
    if (*(unsigned short*)(o + 0xc) != 0xb2) return;

    Vector3 pos;
    Vector3* src = (Vector3*)(((int)o + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    unsigned int flags = *(unsigned int*)(o + 8);
    pos.x = src->x;
    pos.y = src->y;
    pos.z = src->z;

    char* o2 = *(char**)(c + 0x364);
    Vector3_16 rot;
    {
        int b = *(unsigned short*)(o2 + 0x8e);
        int a = *(unsigned short*)(o2 + 0x8c);
        rot.y = b;
        rot.x = a;
        rot.z = *(unsigned short*)(o2 + 0x90);
    }

    ((ActorBase*)*(char**)(c + 0x364))->MarkForDestruction();

    unsigned int newflags = (unsigned char)(flags & 0xf) | 0x20;
    Actor::Spawn(0xb4, newflags, pos, &rot, *(signed char*)(c + 0xcc), -1);
    *(char**)(c + 0x364) = (char*)Actor::Spawn(0xb2, newflags, pos, &rot, *(signed char*)(c + 0xcc), -1);
    *(int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) |= 0x4000000;
}
