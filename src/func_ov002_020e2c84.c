typedef unsigned char u8;
typedef unsigned short u16;

struct Vector3 { int x, y, z; };
struct Camera;

extern void func_ov002_020e301c(char *c);
extern int func_ov002_020e3078(char *self, void *s);
extern int func_ov002_020e2ea0(char *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *state);
extern int func_ov002_020d91e0(char *thiz, int damage, int doPre);
extern void func_0200d8c8(struct Camera *cam, const struct Vector3 *v, int strength);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const struct Vector3 *v);
extern int func_ov002_020c0cbc(char *c);
extern int _ZN6Player7IsStateERNS_5StateE(char *self, void *state);
extern int func_ov002_020c5dec(char *c, int r1);
extern void func_ov002_020db8bc(unsigned char *p, unsigned char val);

extern char data_ov002_02110454;
extern char data_ov002_02110484;
extern char data_ov002_02110094;
extern char data_ov002_0211010c;
extern struct Camera *data_0209f318;

int func_ov002_020e2c84(char *self)
{
    int diff;

    diff = *(int *)(self + 0x684) - *(int *)(self + 0x60);
    func_ov002_020e301c(self);

    if (func_ov002_020e3078(self, &data_ov002_02110454) != 0 ||
        func_ov002_020e3078(self, &data_ov002_02110484) != 0 ||
        *(u8 *)(self + 0x703) != 0)
        return 0;

    if (func_ov002_020e2ea0(self) != 0)
        return 2;

    if (diff > 0xbb8000) {
        *(u8 *)(self + 0x6e3) = 2;
        *(int *)(self + 0x674) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110094);

        if (func_ov002_020d91e0(self, 0x400, 1) != 0) {
            {
                u8 *p = (u8 *)(((long long)(int)(self + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL);
                *p &= 1;
            }
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211010c);
            return 1;
        }

        func_0200d8c8(data_0209f318, (struct Vector3 *)(self + 0x5c), 0x7d0000);
        *(u8 *)(self + 0x70d) = 0;
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 7, (struct Vector3 *)(self + 0x74));
    } else if (diff > 0x47e000) {
        if (func_ov002_020c0cbc(self) == 0) {
            if (func_ov002_020d91e0(self, 0x200, 1) != 0) {
                if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110094) != 0) {
                    {
                        u8 *p = (u8 *)(((long long)(int)(self + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL);
                        *p &= 1;
                    }
                    _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211010c);
                } else {
                    func_ov002_020c5dec(self, 2);
                }
                return 1;
            }

            func_ov002_020db8bc((unsigned char *)self, 1);
            *(int *)(self + 0xa8) = 0;
            func_0200d8c8(data_0209f318, (struct Vector3 *)(self + 0x5c), 0x7d0000);
            *(u8 *)(self + 0x70d) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 7, (struct Vector3 *)(self + 0x74));
        }
    }

    *(int *)(self + 0x684) = *(int *)(self + 0x60);
    return 0;
}
