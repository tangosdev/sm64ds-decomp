extern void Copy36Bytes(int *src, int *dst);

void Geometry_MatrixMultiply3x3(int *m)
{
    *(volatile unsigned int *)0x4000400 = 0x1a;
    Copy36Bytes(m, (int *)0x4000400);
}
