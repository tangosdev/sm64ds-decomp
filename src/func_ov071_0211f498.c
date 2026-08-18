typedef struct { int x, y, z; } Vec3;
extern int _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs();
extern int _ZN8dActor_c8PoofDustEv();
extern int func_02012694();
extern int Scuttlebug_SetState();
extern int _ZN7fBase_c18MarkForDestructionEv();

int func_ov071_0211f498(int *t)
{
    Vec3 v;
    v.x = t[0x17];
    v.y = t[0x18];
    v.z = t[0x19];
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(t, &v, *(unsigned char*)((char*)t + 0x3aa), 0xf000, 0);
    _ZN8dActor_c8PoofDustEv(t);
    func_02012694(0xc4, (char*)t + 0x74);
    if (t[2]) {
        *(unsigned char*)((char*)t + 0x3aa) = 0;
        return Scuttlebug_SetState(t);
    }
    return _ZN7fBase_c18MarkForDestructionEv(t);
}
