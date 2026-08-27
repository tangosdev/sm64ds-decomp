struct Camera;

extern int func_020091f8(struct Camera *, struct Camera *, int, int);
extern void Vec3_RotateYAndTranslate(void *out, int src, short angle,
                                    const int *data);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *dest, const void *src, int factor);
extern void func_02009138(struct Camera *, int);
extern const int data_02086ea8[];

int func_02009540(struct Camera *cam)
{
    /* call func_020091f8(cam, cam+0x120, *(short*)(cam+0x186), 0) */
    func_020091f8(cam,
                  (struct Camera *)((char *)cam + 0x120),
                  *(short *)((char *)cam + 0x186),
                  0);

    /* stack layout: 12-byte temporary vector, then three ints */
    unsigned char tmp_vec[12];
    int tmp_data[3];

    tmp_data[0] = data_02086ea8[0];
    tmp_data[1] = data_02086ea8[1];
    tmp_data[2] = data_02086ea8[2];

    /* Vec3_RotateYAndTranslate(tmp_vec, *(int*)(cam+0x11c),
                               *(short*)(cam+0x186), tmp_data) */
    Vec3_RotateYAndTranslate(tmp_vec,
                             *(int *)((char *)cam + 0x11c),
                             *(short *)((char *)cam + 0x186),
                             tmp_data);

    /* _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(cam+0x120, tmp_vec, 0x8000) */
    _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((void *)((char *)cam + 0x120),
                   (const void *)tmp_vec,
                   0x8000);

    /* func_02009138(cam, 0x300000) */
    func_02009138(cam, 0x300000);

    return 1;
}
