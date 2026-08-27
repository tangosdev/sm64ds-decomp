struct Camera;

extern int data_02086ec0[];
extern void Vec3_RotateYAndTranslate(void *src, void *dst, short angle, void *unk);

int func_0200762c(struct Camera *self)
{
    int stack[3];
    stack[0] = data_02086ec0[0];
    stack[1] = data_02086ec0[1];
    stack[2] = data_02086ec0[2];

    short angle = *(short *)((char *)*(int *)((char *)self + 0x110) + 0x8e);
    angle = (short)(angle + 0x8000);

    Vec3_RotateYAndTranslate(
        (void *)((char *)self + 0xb0),
        (void *)((char *)*(int *)((char *)self + 0x110) + 0x5c),
        angle,
        (void *)stack
    );

    *(int *)((char *)self + 0xb4) = 0x1c9000;
    return 1;
}
