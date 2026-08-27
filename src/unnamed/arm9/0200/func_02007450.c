struct Camera;

extern void Vec3_RotateYAndTranslate(void *dst, void *src, short angle, void *unk);

int func_02007450(struct Camera *cam)
{
    short angle = *(short *)((char *)cam + 0x186);
    Vec3_RotateYAndTranslate((void *)((char *)cam + 0x8c),
                             (void *)((char *)cam + 0x8c),
                             angle,
                             (void *)((char *)cam + 0xb0));
    return 1;
}
