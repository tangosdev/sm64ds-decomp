int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);

int func_ov006_020d2580(char *this, int *p1, int *p2) {
    int hi, lo;
    *(unsigned char*)(this + 0x4709) = 0;
    *(unsigned char*)(this + 0x4708) = 1;
    hi = p1[1];
    lo = p1[0];
    *(int*)(this + 0x46f8) = lo;
    *(int*)(this + 0x46fc) = hi;
    func_ov004_020ae5c4(this, p1[0], p1[1], p2[0], p2[1],
                        *(int*)(this + 0x53e4), 1);
    *(unsigned char*)(this + 0x4708) = 0;
    return *(unsigned char*)(this + 0x4709);
}
