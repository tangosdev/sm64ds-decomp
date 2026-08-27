extern void func_ov073_021223a4(char *c, void *p);
extern int data_ov073_021234b0;
extern int data_ov073_021234c0;
extern int data_ov073_021234d0;
void func_ov073_02122730(char *r0, char *r1)
{
    int b = (int)(*(unsigned short*)(r1 + 0xc) == 0xda);
    if (b == 0) return;
    if (*(int*)(r0 + 0x320) != (int)&data_ov073_021234b0) return;
    *(unsigned char*)(r1 + 0x4c4) = *(unsigned char*)(r0 + 0x32d);
    if (*(unsigned char*)(r0 + 0x32d) != *(unsigned char*)(r1 + 0x4c5)) return;
    if (*(unsigned char*)(r0 + 0x32c) != 0)
        func_ov073_021223a4(r0, &data_ov073_021234c0);
    else
        func_ov073_021223a4(r0, &data_ov073_021234d0);
}
