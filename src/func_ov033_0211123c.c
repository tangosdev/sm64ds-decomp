// @symbol func_ov033_0211123c
// @emits daObjTtFuta_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* daObjTtFuta_c::OnGroundPounded - recovered from vtable slot identity */
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void *v);
extern void _ZN5Event6SetBitEj(unsigned int b);
extern void _ZN9ActorBase18MarkForDestructionEv(void *c);

void daObjTtFuta_c_OnGroundPounded(char *self, char *other)
{
    int *v = (int *)(((int)other + 0x5c) & 0xFFFFFFFFFFFFFFFF);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x28, v[0], v[1], v[2]);
    _ZN5Sound9PlayBank3EjRK7Vector3(0xf, self + 0x74);
    _ZN5Event6SetBitEj(0xe);
    _ZN9ActorBase18MarkForDestructionEv(self);
}
