struct Camera;

extern signed char data_0209f2f8;
extern unsigned char data_0209f224;

extern int SublevelToLevel(int i);
extern void func_020071a8(struct Camera *cam, short ang, int threshold);

int func_020077f0(struct Camera *cam)
{
    struct Camera *r4 = cam;
    int lvl = SublevelToLevel(data_0209f2f8);

    if (lvl == 2) {
        if (data_0209f224 == 5) {
            func_020071a8(r4, 0, 0x4000);
            goto end;
        }
    }

    if (lvl == 8) {
        if (data_0209f224 == 1) {
            func_020071a8(r4, (short)0x8000, 0x5000);
        }
    }

end:
    return 1;
}
