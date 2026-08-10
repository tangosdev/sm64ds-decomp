//cpp
// @symbol func_ov062_0211a740
// recovered name: Koopa_Kill
/* recovered: shared common types, renamed to Class_Method */
/* daNknk_c::Kill - recovered from vtable slot identity */
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
struct Vector3 { int x, y, z; };
extern "C" {
int _Z14ApproachLinearRsss(s16* v, s16 target, s16 step);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* file, int i, int fix, unsigned int a);
void func_0201267c(int a, void* p);
void func_02012790(int a);
int _ZN6Player12GetTalkStateEv(void* self);
int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(void* self, void* actor, unsigned int msg, const Vector3* v, unsigned int a, unsigned int b);
int _ZN6Player18HasFinishedTalkingEv(void* self);
void _ZN5Timer10ResetTimerEv(void* self);
}
extern int data_ov062_0211e03c[];
extern int data_ov062_0211e034[];
extern s8 data_0209f2f8;
extern u8 data_0209d684;
extern char data_0209d4c8[];
extern "C" void func_ov062_0211a740(char* c)
{
    switch (*(u8*)(c + 0x390)) {
    case 0:
        if (_Z14ApproachLinearRsss((s16*)(c + 0x94), *(s16*)(c + 0x3a8), 0x800) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), (void*)data_ov062_0211e03c[1], 0, 0x1000, 0);
            *(u8*)(c + 0x390) += 1;
            func_0201267c(0xec, c + 0x74);
            if (*(int*)(*(int*)(c + 0x398) + 8) == 0) {
                func_02012790(0xa);
                *(u8*)(c + 0x3b6) = 1;
            }
        }
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        return;
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x398)) != 0)
            return;
        {
            unsigned int msg;
            Vector3 v;
            if (*(u8*)(c + 0x3b6) == 0) {
                msg = 0x9e;
            } else if (data_0209f2f8 == 0x18) {
                msg = 0xc4;
            } else {
                msg = 0x91;
            }

            {
                int z = *(int*)(c + 0x64);
                int y = *(int*)(c + 0x60) + 0xc8000;
                int x = *(int*)(c + 0x5c);
                v.x = x;
                v.y = y;
                v.z = z;
            }
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(*(void**)(c + 0x398), c, msg, &v, 1, 0) != 0)
                *(u8*)(c + 0x390) += 1;
        }
        return;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x398)) != 2)
            return;
        if (*(u8*)(c + 0x3b6) != 0) {
            if (data_0209d684 == 1) {
                *(int*)(c + 0x38c) = 2;
                *(u16*)(c + 0x100) = 0x32;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), (void*)data_ov062_0211e034[1], 0, 0x1000, 0);
                *(u16*)(c + 0x3aa) = 0;
                *(u8*)(c + 0x3b6) = 0;
            } else if (data_0209d684 == 2) {
                *(int*)(c + 0x38c) = 0;
                *(u16*)(c + 0x100) = 0x3c;
                _ZN6Player18HasFinishedTalkingEv(*(void**)(c + 0x398));
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), (void*)data_ov062_0211e034[1], 0, 0x1000, 0);
                *(u8*)(c + 0x3b6) = 0;
            }
        } else {
            *(int*)(c + 0x38c) = 0;
            *(u16*)(c + 0x100) = 0x3c;
            _ZN6Player18HasFinishedTalkingEv(*(void**)(c + 0x398));
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), (void*)data_ov062_0211e034[1], 0, 0x1000, 0);
        }
        *(u8*)(c + 0x390) = 0;
        *(int*)(c + 0x98) = 0;
        _ZN5Timer10ResetTimerEv(data_0209d4c8);
        return;
    default:
        return;
    }
}
