struct dActor_c;
extern struct dActor_c *_ZN8dActor_c13ClosestPlayerEv(void);
extern signed char data_0209f2f8;
extern int data_0209f32c;

int func_ov102_02149078(struct dActor_c *self)
{
    if (data_0209f2f8 == 0x15) {
        if ((int)(*(int*)((char*)self + 0x3e0) - 0x32000) <= data_0209f32c)
            return 1;
    } else {
        if (data_0209f2f8 == 0x21)
            return 0;
        if (*(unsigned char*)((char*)_ZN8dActor_c13ClosestPlayerEv() + 0x706))
            return 1;
    }
    return 0;
}
