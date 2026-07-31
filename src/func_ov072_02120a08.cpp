//cpp
// @symbol func_ov072_02120a08
// @emits daBgSnwmn_c_Behavior
/* recovered: shared common types, renamed to Class_Method */
/* daBgSnwmn_c::Behavior - recovered from vtable slot identity */
struct Vector3 { int x, y, z; };
extern "C" {
void _ZN9Animation7AdvanceEv(void *self);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
}
extern const Vector3 data_ov072_02122c70;
extern "C" int daBgSnwmn_c_Behavior(char *c)
{
    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1b0, &data_ov072_02122c70);
    _ZN12CylinderClsn5ClearEv(c + 0x1b0);
    _ZN12CylinderClsn6UpdateEv(c + 0x1b0);
    return 1;
}
