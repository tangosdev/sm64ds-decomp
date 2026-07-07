typedef int Fix12i;
struct Vector3 { Fix12i x, y, z; };
struct Matrix3x3_Vecs { struct Vector3 v0, v1, v2; };

struct Part {
    int count;
    unsigned char* cmd;
    int f8;
    int fc;
};
struct Entry {
    int count;
    struct Part* parts;
};

extern void Geometry_MatrixMultiply3x3(void* m);
extern void func_020458a8(struct Matrix3x3_Vecs* dst, struct Matrix3x3_Vecs* src);
extern void func_020553a4(void* m);
extern void func_02055388(void* m);
extern void MulMat3x3Mat3x3(void* a, void* b, void* c);
extern void func_02052514(void* dst, void* src);
extern void func_02055998(int* m);
extern void func_01ffde98(int a, int b, int c);

extern void* data_020a4bd0;
extern int data_020a4bd4;
extern unsigned char data_020a4bbc;
extern short data_020a4bc0;
extern short data_020a4bc4;

void func_0204488c(char* self, int index, int* color)
{
    int M[9];
    int N[16];
    int i;
    struct Part* part;
    struct Entry* e;
    int j;
    int fp_index;
    unsigned char* cmd;
    void* partMtx;
    void* lastMat;
    volatile int* mtxScale;

    e = &(*(struct Entry**)(*(char**)self + 0x10))[index];
    part = e->parts;

    mtxScale = (volatile int*)0x400046c;

    for (i = 0; i < e->count; i++, part++) {
        cmd = part->cmd;
        for (j = 0; j < part->count; j++, cmd++) {
            unsigned char b = *cmd;
            int check;
            if (b == 0xff)
                continue;

            fp_index = (*(unsigned short**)(*(char**)self + 0x2c))[b];
            partMtx = (char*)*(void**)(self + 0xc) + fp_index * 0x30;

            *(volatile int*)0x4000440 = 2;
            *(volatile int*)0x4000454 = 0;
            Geometry_MatrixMultiply3x3(data_020a4bd0);

            check = *(unsigned short*)((char*)*(void**)(self + 8) + fp_index * 0x34 + 0x18);
            if (check) {
                func_020458a8((struct Matrix3x3_Vecs*)partMtx, (struct Matrix3x3_Vecs*)M);
                Geometry_MatrixMultiply3x3(M);
                lastMat = M;
            } else {
                Geometry_MatrixMultiply3x3(partMtx);
                lastMat = partMtx;
            }

            *(volatile int*)0x4000440 = 1;
            func_020553a4(data_020a4bd0);
            if (color) {
                int c2 = color[2];
                int c1 = color[1];
                int c0 = color[0];
                *mtxScale = c0;
                *mtxScale = c1;
                *mtxScale = c2;
            }
            func_02055388(partMtx);
            {
                int white = data_020a4bd4;
                *mtxScale = white;
                *mtxScale = white;
                *mtxScale = white;
            }
            *(volatile int*)0x400044c = j;
        }

        if (data_020a4bbc == 2) {
            int sx;
            int sy;
            MulMat3x3Mat3x3(lastMat, data_020a4bd0, M);
            func_02052514(M, N);
            sx = data_020a4bc0;
            N[0] = M[0] * sx;
            N[4] = M[3] * sx;
            N[8] = M[6] * sx;
            sy = data_020a4bc4;
            N[1] = M[1] * sy;
            N[5] = M[4] * sy;
            N[9] = M[7] * sy;
            func_02055998(N);
            *(volatile int*)0x400044c = 0;
        }

        func_01ffde98(3, part->fc, part->f8);
    }
}
