extern unsigned short func_ov006_0211507c(unsigned char* base, int* pos);

int func_ov006_02112504(unsigned char** obj, int* p)
{
    int local[2];
    unsigned short v;
    int x, z, diff;

    local[0] = p[0];
    local[1] = p[1];
    v = func_ov006_0211507c(obj[1], local);

    if (v == 0x24d) {
        z = (p[1] >> 12) & 7;
        x = (p[0] >> 12) & 7;
        diff = x - z;
        if (x + z >= 8 && diff >= 0) return 1;
    } else if (v == 0x24e) {
        z = (p[1] >> 12) & 7;
        x = (p[0] >> 12) & 7;
        diff = x - z;
        if (x + z < 8 && diff < 0) return 1;
    } else if (v == 0x24f) {
        z = (p[1] >> 12) & 7;
        x = (p[0] >> 12) & 7;
        diff = x - z;
        if (x + z >= 4 && x + z < 12 && diff >= -4 && diff < 4) return 1;
    } else if (v == 0x28f) {
        x = (p[0] >> 12) & 7;
        if (x >= 1 && x < 7) return 1;
    } else if (v == 0x2af) {
        x = (p[0] >> 12) & 7;
        if (x >= 1 && x < 7) return 1;
    }
    return 0;
}
