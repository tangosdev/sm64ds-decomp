extern int data_ov006_0213c958;
extern int data_0209e650;
extern int data_ov006_0213ca0c[];
extern int _ZN4cstd4fdivEii(int, int);
extern int RandomIntInternal(int* seed);
extern void func_ov006_020ec2bc(void* self);

typedef struct { int x, y; } Vec2;

#pragma opt_strength_reduction off

void func_ov006_020ec9c0(char* self, int idx)
{
    int n1;
    int A, B;
    int p;
    int valY;
    int pos[2];
    int i;
    int off;

    *(int*)(self + 0x40) = 0xa00;
    *(int*)(self + 0x44) = 0xa00;

    n1 = data_ov006_0213c958 - 1;
    A = _ZN4cstd4fdivEii(idx << 12, n1 << 12);
    B = _ZN4cstd4fdivEii((n1 - idx) << 12, n1 << 12);

    p = (n1 - 2) << 3;
    valY = (int)(((long long)((0x28 - p) << 12) * A + 0x800) >> 12)
         + (int)(((long long)((p + 0x98) << 12) * B + 0x800) >> 12);

    *(int*)((char*)pos + 4) = 0;
    *(int*)((char*)pos + 0) = 0;
    *(int*)((char*)pos + 4) = valY;

    if (RandomIntInternal(&data_0209e650) & 0x8000000) {
        *(short*)(self + 0x76) = -0x4000;
        pos[0] = 0x80000;
        pos[0] += ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    } else {
        *(short*)(self + 0x76) = 0x4000;
        pos[0] = 0x80000;
        pos[0] -= ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    }

    for (i = 0, off = 0; i < 5; i++) {
        *(int*)(self + i * 8 + 0x18) = pos[0];
        *(int*)(self + i * 8 + 0x1c) = pos[1];
        if (*(short*)(self + 0x76) < 0)
            *(int*)(self + i * 8 + 0x18) += off;
        else
            *(int*)(self + i * 8 + 0x18) -= off;
        *(short*)(self + i * 2 + 0x7a) = *(short*)(self + 0x76);
        off += 0x20000;
    }

    func_ov006_020ec2bc(self);
    *(int*)(self + 0x68) = 0;
    *(int*)(self + 0x6c) = 0x1ec;
    *(Vec2*)(self + 0) = *(Vec2*)data_ov006_0213ca0c;
    *(Vec2*)(self + 8) = *(Vec2*)(self + 0);
}
