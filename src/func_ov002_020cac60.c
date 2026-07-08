extern unsigned char NumStars(void);
extern short GetAngleToCamera(int i);
extern void LoadLevel(int a, int b, int c, int d, int e);
extern void _ZN5Scene20SetAndStopColorFaderEv(void);
extern void func_02012790(int a);

extern short DEFAULT_SCENE_FADER;

int func_ov002_020cac60(char *self)
{
    short ang;
    short angle;
    if (*(int *)(self + 0x664) == 0xd && NumStars() >= 0xe) {
        ang = *(short *)(self + 0x766);
        angle = GetAngleToCamera(*(unsigned char *)(self + 0x6d8));
        if (ang <= 0x1c00 && angle >= -0x1000 && angle <= 0x1000) {
            LoadLevel(0x1f, 0, 1, 0, 0xe);
            _ZN5Scene20SetAndStopColorFaderEv();
            *(short *)((char *)&DEFAULT_SCENE_FADER + 0xc) = 0x7fff;
            func_02012790(0x1e);
            return 1;
        }
    }
    return 0;
}
