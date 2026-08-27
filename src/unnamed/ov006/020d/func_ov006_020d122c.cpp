//cpp
typedef unsigned int u32;

extern "C" int RandomIntInternal(int *seed);
extern "C" int data_0209d4b8;

#pragma opt_strength_reduction off

struct O {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual int v36();
};

#define A714(i) (((int *)(self + 0x4714))[i])
#define A46A4(i) (((int *)(self + 0x46a4))[i])
#define A4694(i) (((int *)(self + 0x4694))[i])

extern "C" void func_ov006_020d122c(char *self, int arg1)
{
    int i;
    int k;
    int j;
    int t;
    int u;
    int *pi;

    if (((O *)self)->v36()) {
        switch (*(int *)(self + 0x53d4)) {
        case 0:
            A714(0) = 0;
            A714(1) = 0;
            A714(2) = 3;
            A714(3) = 3;
            break;
        case 1:
        case 3:
        case 5:
            A714(0) = 0;
            A714(1) = 0;
            A714(2) = 1;
            A714(3) = 3;
            break;
        default:
            for (i = 0; i < 4; i++)
                A714(i) = i;
            break;
        }
        for (i = 0; i < 4; i++)
            A46A4(i) = A714(i);
        {
            int off = 0x4714;
            arg1 = 1;
            do {
                for (i = 0; i < 4; i++) {
                    t = RandomIntInternal(&data_0209d4b8);
                    pi = (int *)(self + (i << 2) + off);
                    j = ((u32)(((u32)t >> 16) & 0x7fff) * 4) >> 15;
                    t = *pi;
                    {
                        int *pjx = (int *)(self + (j << 2) + off);
                        u = *pjx;
                        *pi = u;
                        *pjx = t;
                    }
                }
                for (k = 0; k < 4; k++) {
                    if (A46A4(k) != A714(A4694(k))) {
                        arg1 = 0;
                        break;
                    }
                }
            } while (arg1 == 1);
        }
    } else {
        if (arg1 > 4)
            arg1 = 4;
        for (k = 0; k < arg1; k++)
            A714(k) = 1;
        for (; arg1 < 4; arg1++)
            A714(arg1) = 0;
        {
            int off = 0x4714;
            for (i = 0; i < 4; i++) {
                t = RandomIntInternal(&data_0209d4b8);
                pi = (int *)(self + (i << 2) + off);
                j = ((u32)(((u32)t >> 16) & 0x7fff) * 4) >> 15;
                t = *pi;
                {
                    int *pjx = (int *)(self + (j << 2) + off);
                    u = *pjx;
                    *pi = u;
                    *pjx = t;
                }
            }
        }
    }
}
