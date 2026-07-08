struct Camera;

extern signed char LEVEL_ID;
extern unsigned char STAR_OBTAINED_COPY;

extern int SublevelToLevel(int i);
extern void func_020071a8(struct Camera *cam, short ang, int threshold);

int func_020077f0(struct Camera *cam)
{
    struct Camera *r4 = cam;
    int lvl = SublevelToLevel(LEVEL_ID);

    if (lvl == 2) {
        if (STAR_OBTAINED_COPY == 5) {
            func_020071a8(r4, 0, 0x4000);
            goto end;
        }
    }

    if (lvl == 8) {
        if (STAR_OBTAINED_COPY == 1) {
            func_020071a8(r4, (short)0x8000, 0x5000);
        }
    }

end:
    return 1;
}
