extern long long func_0203d5bc(int* p);
extern int func_ov006_02114590(int a0, int* p, int* q0, int* q1, int* q2);

typedef struct {
    char _0[8];
    int x, y;        /* 0x8, 0xc */
    char _10[0x30];  /* 0x10..0x3f */
    int f40;         /* 0x40 */
    char _44[4];     /* 0x44 */
    int verts[16];   /* 0x48 */
} State;

int func_ov006_0210dbb0(State* s, int* p) {
    int v[2];
    int b2p[2], b2q0[2], b2q1[2], b2q2[2];
    int b3p[2], b3q0[2], b3q1[2], b3q2[2];
    int b4p[2], b4q0[2], b4q1[2], b4q2[2];
    int b5p[2], b5q0[2], b5q1[2], b5q2[2];

    if (s->f40 <= 1 && p[1] < s->y + 0x8000) {
        v[0] = p[0];
        v[1] = p[1];
        v[0] -= s->x;
        v[1] -= s->y;
        v[1] -= 0x8000;
        if (func_0203d5bc(v) < 0x100000) return 1;
    }

    b2p[0] = p[0]; b2p[1] = p[1];
    b2q0[0] = s->verts[0]; b2q0[1] = s->verts[1];
    b2q1[0] = s->verts[2]; b2q1[1] = s->verts[3];
    b2q2[0] = s->verts[4]; b2q2[1] = s->verts[5];
    if (func_ov006_02114590((int)s, b2p, b2q0, b2q1, b2q2)) return 1;

    b3p[0] = p[0]; b3p[1] = p[1];
    b3q0[0] = s->verts[2]; b3q0[1] = s->verts[3];
    b3q1[0] = s->verts[4]; b3q1[1] = s->verts[5];
    b3q2[0] = s->verts[6]; b3q2[1] = s->verts[7];
    if (func_ov006_02114590((int)s, b3p, b3q0, b3q1, b3q2)) return 1;

    b4p[0] = p[0]; b4p[1] = p[1];
    b4q0[0] = s->verts[8];  b4q0[1] = s->verts[9];
    b4q1[0] = s->verts[10]; b4q1[1] = s->verts[11];
    b4q2[0] = s->verts[12]; b4q2[1] = s->verts[13];
    if (func_ov006_02114590((int)s, b4p, b4q0, b4q1, b4q2)) return 1;

    b5p[0] = p[0]; b5p[1] = p[1];
    b5q0[0] = s->verts[10]; b5q0[1] = s->verts[11];
    b5q1[0] = s->verts[12]; b5q1[1] = s->verts[13];
    b5q2[0] = s->verts[14]; b5q2[1] = s->verts[15];
    return func_ov006_02114590((int)s, b5p, b5q0, b5q1, b5q2);
}
