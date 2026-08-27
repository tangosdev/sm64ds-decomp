extern void func_0200cb58(void *obj, int index);
extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, const int *ptr);
extern int data_02086eb4[];

int func_020094e4(void *cam)
{
    int buf[3];
    func_0200cb58(cam, 0x1f);
    buf[0] = data_02086eb4[0];
    buf[1] = data_02086eb4[1];
    buf[2] = data_02086eb4[2];
    short angle = *(short *)((char *)*(int *)((char *)cam + 0x110) + 0x8e);
    Vec3_RotateYAndTranslate((char *)cam + 0x8c, (char *)cam + 0x98, angle, buf);
    return 1;
}
