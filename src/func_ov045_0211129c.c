// @symbol func_ov045_0211129c
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjKm2_Agaru_c.h"
// @emits daObjKm2_Agaru_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjKm2_Agaru_c::Behavior - recovered from vtable slot identity */
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
int daObjKm2_Agaru_c_Behavior(char *c)
{
    struct daObjKm2_Agaru_c *self = (struct daObjKm2_Agaru_c *)(void *)c;
    switch (self->unk_327) {
    case 0:
        if (self->unk_326 != 0)
            *(unsigned char*)(((int)c + 0x327)) =
                *(unsigned char*)(((int)c + 0x327)) + 1;
        break;
    case 1:
        if (self->unk_324 >= 0x14) {
            int lim;
            *(int*)(((int)c + 0x60)) =
                *(int*)(((int)c + 0x60)) + 0xa000;
            lim = self->unk_320 + 0x5dc000;
            if (self->unk_060 >= lim) {
                self->unk_060 = lim;
                *(unsigned char*)(((int)c + 0x327)) =
                    *(unsigned char*)(((int)c + 0x327)) + 1;
                self->unk_324 = 0;
            }
        } else {
            *(unsigned short*)(((int)c + 0x324)) =
                *(unsigned short*)(((int)c + 0x324)) + 1;
        }
        break;
    case 2:
        if (self->unk_324 >= 0x14) {
            int lim;
            *(int*)(((int)c + 0x60)) =
                *(int*)(((int)c + 0x60)) - 0xa000;
            lim = self->unk_320;
            if (self->unk_060 <= lim) {
                self->unk_060 = lim;
                self->unk_327 = 0;
                self->unk_324 = 0;
            }
        } else {
            *(unsigned short*)(((int)c + 0x324)) =
                *(unsigned short*)(((int)c + 0x324)) + 1;
        }
        break;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x1f4000, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    self->unk_326 = 0;
    return 1;
}
