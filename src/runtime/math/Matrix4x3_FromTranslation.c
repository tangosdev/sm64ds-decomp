struct Mtx43 { int a[12]; };
void Matrix4x3_FromTranslation(struct Mtx43* m, int x, int y, int z) {
    m->a[0] = 0x1000;
    m->a[1] = 0;
    m->a[2] = 0;
    m->a[3] = 0;
    m->a[4] = 0x1000;
    m->a[5] = 0;
    m->a[6] = 0;
    m->a[7] = 0;
    m->a[8] = 0x1000;
    m->a[9] = x;
    m->a[10] = y;
    m->a[11] = z;
}
