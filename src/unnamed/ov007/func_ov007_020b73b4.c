extern void func_ov007_020c3d1c(void *p);
extern void func_ov007_020c105c(void *p);
extern void func_ov007_020c05f8(void *p);
extern void func_ov007_020c8b04(void *p);
extern void func_ov007_020c3544(void *p);
extern void func_ov007_020c338c(void *p);
extern void func_ov007_020c121c(void *p);
extern void func_ov007_020c1180(void *p);
extern void func_ov007_020c0a9c(void *p);

void func_ov007_020b73b4(int *self)
{
    void **arr;
    int i;

    func_ov007_020c3d1c((void *)self[4]);
    func_ov007_020c3d1c((void *)self[5]);
    func_ov007_020c105c((void *)self[0]);
    func_ov007_020c105c((void *)self[1]);
    func_ov007_020c05f8((void *)self[2]);
    func_ov007_020c05f8((void *)self[3]);

    if (self[6] > 0) {
        arr = (void **)self[7];
        for (i = 0; i < self[6]; i++)
            if (arr[i]) func_ov007_020c8b04(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (self[8] > 0) {
        arr = (void **)self[9];
        for (i = 0; i < self[8]; i++)
            if (arr[i]) func_ov007_020c3544(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (self[10] > 0) {
        arr = (void **)self[11];
        for (i = 0; i < self[10]; i++)
            if (arr[i]) func_ov007_020c338c(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (self[14] > 0) {
        arr = (void **)self[15];
        for (i = 0; i < self[14]; i++)
            if (arr[i]) func_ov007_020c121c(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (self[16] > 0) {
        arr = (void **)self[17];
        for (i = 0; i < self[16]; i++)
            if (arr[i]) func_ov007_020c1180(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    if (self[12] > 0) {
        arr = (void **)self[13];
        for (i = 0; i < self[12]; i++)
            if (arr[i]) func_ov007_020c0a9c(arr[i]);
        func_ov007_020c3d1c(arr);
    }

    func_ov007_020c3d1c((void *)self);
}
