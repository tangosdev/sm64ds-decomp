//cpp
// @symbol _ZN11daBgSnwmn_c8BehaviorEv
/* daBgSnwmn_c::Behavior() -- vtable slot 6. See include/daBgSnwmn_c.h. All
 * three calls reach into the typed mTexSeq/mCylClsn members. */
struct Vector3 { int x, y, z; };
extern "C" {
void _ZN9Animation7AdvanceEv(void *self);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
void _ZN5dCc_c5ClearEv(void *self);
void _ZN5dCc_c6UpdateEv(void *self);
}
extern const Vector3 data_ov072_02122c70;
extern "C" int _ZN11daBgSnwmn_c8BehaviorEv(char *c)
{
    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x1b0, &data_ov072_02122c70);
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}
