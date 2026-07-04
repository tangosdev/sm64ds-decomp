typedef int s32;
typedef unsigned int u32;
typedef short s16;

extern int _ZN9Animation8FinishedEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int i, int fix, u32 j);
extern int _Z14ApproachLinearRsss(s16 *a, s16 b, s16 c);
extern void func_ov006_020cc698(char *c);

extern void *data_ov006_021405c8[];
extern void *data_ov006_0214057c;
extern void *data_ov006_02140564;
extern void *data_ov006_0214056c;
extern void *data_ov006_02140568;

void func_ov006_020cc724(char *c)
{
    *(void **)(c + 0x20) = data_ov006_021405c8[*(s16 *)(c + 0x52)];

    if (*(void **)(c + 0xcc) == data_ov006_0214057c) {
        if (_ZN9Animation8FinishedEv(c + 0xbc) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, data_ov006_02140564, 0, 0x800, 0);
            return;
        }
    }

    if (*(void **)(c + 0xcc) == data_ov006_02140564) {
        if (*(s32 *)(c + 0x34) > 0) {
            if (*(s32 *)(c + 0x1c) > 0x70000) {
                *(s32 *)(c + 0x1c) = 0x70000;
                *(s32 *)(c + 0x34) = 0;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, data_ov006_0214056c, 0x40000000, 0x800, 0);
            }
            return;
        }
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x4a), 0x4000, 0x800) != 0)
            *(s32 *)(c + 0x34) = 0x1000;
        return;
    }

    if (*(void **)(c + 0xcc) == data_ov006_0214056c) {
        if (_ZN9Animation8FinishedEv(c + 0xbc) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, data_ov006_02140568, 0x40000000, 0x800, 0);
            return;
        }
    }

    if (*(void **)(c + 0xcc) == data_ov006_02140568) {
        if (_ZN9Animation8FinishedEv(c + 0xbc) != 0)
            func_ov006_020cc698(c);
    }
}
