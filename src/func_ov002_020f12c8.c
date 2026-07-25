typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern u8 IsAreaShowing(s32 idx);
extern s32 _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, s32 vol);
extern void _ZN5Event6SetBitEj(u32 bit);
extern void _ZN16MeshColliderBase7DisableEv(void *self);
extern void _ZN5Actor8PoofDustEv(void *self);
extern u16 DecIfAbove0_Short(u16 *p);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
extern s32 func_02012310(s32 a, s32 b, s32 c);
extern void *_ZN5Actor15FindWithActorIDEjPS_(u32 id, void *prev);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern s32 _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, s32 a, s32 b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);

s32 func_ov002_020f12c8(void *arg0)
{
    char *c = (char *)arg0;
    u16 t;

    if (*(u8 *)(c + 0x32c) == 1) {
        *(s8 *)(c + 0xcc) = -1;
        if (IsAreaShowing(*(s8 *)(c + 0x32e)) == 0)
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x3f000);
    }
    if (*(u8 *)(c + 0x32c) == 1) {
        if (*(s32 *)(c + 0x60) > *(s32 *)(c + 0x320)) {
            *(s32 *)((int)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL)) =
                *(s32 *)((int)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL)) - 0x14000;
            if (*(s32 *)(c + 0x60) <= *(s32 *)(c + 0x320)) {
                *(s32 *)(c + 0x60) = *(s32 *)(c + 0x320);
                _ZN5Event6SetBitEj(*(u8 *)(c + 0x32d));
                *(u16 *)(c + 0x328) = *(u16 *)(c + 0x32a);
                _ZN16MeshColliderBase7DisableEv(c + 0x124);
                _ZN5Actor8PoofDustEv(c);
            }
        }
        if (*(u16 *)(c + 0x328) != 0) {
            if (DecIfAbove0_Short((u16 *)(c + 0x328)) == 0) {
                *(u16 *)(c + 0x328) = 1;
                if (_ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x64cc) != 0)
                    _ZN5Actor24KillAndTrackInDeathTableEv(c);
            } else {
                t = *(u16 *)(c + 0x328);
                if (t == 0x2d)
                    *(s32 *)(c + 0x324) = 0;
                else if (t < 0x2d)
                    *(s32 *)(c + 0x324) = func_02012310(*(s32 *)(c + 0x324), 0x39, 0);
                else
                    *(s32 *)(c + 0x324) = func_02012310(*(s32 *)(c + 0x324), 0x38, 0);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0xc999);
                if (_ZN5Actor15FindWithActorIDEjPS_(0x122, 0) == 0)
                    *(u16 *)(c + 0x328) = 1;
            }
        }
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x1f4000, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
