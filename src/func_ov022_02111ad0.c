// @symbol func_ov022_02111ad0
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjFl_London_c.h"
// recovered name: daObjFl_London_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjFl_London_c::Behavior - recovered from vtable slot identity */
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void* v);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* t, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* t);

int func_ov022_02111ad0(char* c)
{
    struct daObjFl_London_c *self = (struct daObjFl_London_c *)(void *)c;
    if (DecIfAbove0_Byte((unsigned char*)c + 0x31e) == 0) {
        if (self->unk_31f == 0) {
            short* p = (short*)((long long)(int)(c + 0x96));
            *p = *p - 0x100;
            if (self->unk_096 <= -0x2000) {
                self->unk_096 = -0x2000;
                self->unk_31e = 0xf;
                self->unk_31f = 1;
            }
        } else {
            short* p = (short*)((long long)(int)(c + 0x96));
            *p = *p + 0x100;
            if (self->unk_096 >= 0) {
                self->unk_096 = 0;
                self->unk_31e = 0xf;
                self->unk_31f = 0;
            }
        }
    } else {
        if (self->unk_31e == 1) {
            if (self->unk_31f == 0)
                _ZN5Sound9PlayBank3EjRK7Vector3(0x34, c + 0x74);
            else
                _ZN5Sound9PlayBank3EjRK7Vector3(0x35, c + 0x74);
        }
    }
    self->unk_090 = self->unk_096;
    func_ov022_02111a1c(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
