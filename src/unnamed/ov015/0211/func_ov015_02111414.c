extern short Vec3_HorzAngle(const void* v0, const void* v1);
extern unsigned short AngleDiff(short a, short b);
void func_ov015_02111414(char* c, char* other) {
    int d;
    short ang;
    if (*(unsigned char*)(c+0x397) >= 2) return;
    if (*(unsigned char*)(c+0x397) == 0)
        (*(unsigned char*)(long long)(((int)c + 0x397)))++;
    ang = Vec3_HorzAngle(c+0x5c, other+0x5c);
    d = AngleDiff(ang, *(short*)(c+0x8e));
    if (d < 0x2000) {
        *(char*)(c+0x396) = ~0;
    } else {
        if (d < 0x2000) goto set1;
        if (d < 0x6000) return;
set1:
        *(unsigned char*)(c+0x396) = 1;
    }
    if (*(unsigned char*)(c+0x397) == 1) {
        if (*(short*)(c+0x394) >= 0x320) {
            if (*(int*)(other+0x60) > *(int*)(c+0x60) + 0x64000)
                (*(unsigned char*)(long long)(((int)c + 0x397)))++;
        }
    }
    *(short*)(c+0x394) = 0x640;
}
