#pragma opt_strength_reduction off
typedef void (*VFunc)(void*);

extern void func_ov006_02115480(char* c);
extern void func_ov006_02115150(char* c);
extern void func_ov006_02115a5c(char* c);
extern int func_ov006_021156f8(char* c);
extern int func_ov006_02111d74(char* c);
extern void func_ov004_020ae20c(void);
extern void func_ov004_020b0aa0(int a);
extern int func_ov006_02111dcc(char* p, int val);
extern void func_ov006_02111e48(char* o);
extern void func_ov006_0211470c(int* a, char* b);
extern void func_ov006_02115598(char* c, int* src, int v2, int v3, int v5);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int s);
extern void func_ov006_02114f98(char* self);
extern void func_ov004_020b0a54(int a);
extern unsigned int func_02012790(unsigned int a);
extern void func_ov006_02114c04(char* c);

extern unsigned char data_0209d454;
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern int func_020bc888;
extern int func_020bc864;

int func_ov006_02118488(void* arg)
{
    char* c = (char*)arg;
    int vec[3];

    if (*(unsigned char*)(c + 0xc4) == 0) {
        c[0xc3] = 1;
        c[0xc4] = 1;
        *(short*)(c + 0xc0) = 0;
    }

    switch (*(int*)(c + 0x4660)) {
    case 0:
        {
            char* o;
            int i;

            o = *(char**)(c + 0x4684);
            (**(VFunc**)o)(o);

            for (i = 0; i < *(int*)(c + 0x4668); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x4688)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }
            for (i = 0; i < *(int*)(c + 0x4670); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x46bc)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }
            for (i = 0; i < *(int*)(c + 0x466c); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x4720)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }
            for (i = 0; i < *(int*)(c + 0x4674); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x4740)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }
            for (i = 0; i < *(int*)(c + 0x4678); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x474c)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }
            for (i = 0; i < *(int*)(c + 0x467c); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x4764)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }
            for (i = 0; i < *(int*)(c + 0x4680); i++) {
                o = *(char**)(((long long)(int)(c + i * 4 + 0x4770)) & 0xFFFFFFFFFFFFFFFFLL);
                (**(VFunc**)o)(o);
            }

            o = *(char**)(c + 0x4778);
            if (o != 0) {
                (**(VFunc**)o)(o);
            }
            o = *(char**)(c + 0x477c);
            if (o != 0) {
                (**(VFunc**)o)(o);
            }
            o = *(char**)(c + 0x4780);
            if (o != 0) {
                (**(VFunc**)o)(o);
            }

            func_ov006_02115480(c);
            func_ov006_02115150(c);
            func_ov006_02115a5c(c);

            if (func_ov006_021156f8(c) != 0) {
                if (*(int*)(c + 0x5954) >= 0x3c) {
                    *(int*)(c + 0x4660) = 1;
                } else {
                    int* p = (int*)(((int)c + 0x5954) & 0xFFFFFFFFFFFFFFFF);
                    (*p)++;
                }
            } else {
                int cur;
                int next;
                *(int*)(c + 0x5954) = 0;
                cur = *(int*)(c + 0x4664);
                next = cur + 1;
                if (next >= *(int*)(c + 0x4668)) {
                    next = 0;
                }
                if (func_ov006_02111d74(((char**)(c + 0x4688))[cur]) != 0) {
                    *(int*)(c + 0x4664) = next;
                }
            }

            if (*(unsigned char*)(c + 0x595c) != 0) {
                int t;
                {
                    int* p = (int*)(((int)c + 0x5958) & 0xFFFFFFFFFFFFFFFF);
                    (*p)++;
                }
                if (*(int*)(c + 0x5958) == 0xb4) {
                    func_ov004_020ae20c();
                    func_ov004_020b0aa0(0x1d);
                }
                t = *(int*)(c + 0x5958);
                if (t >= 0xb4 && t % 0x1e == 0) {
                    int found = 0;
                    for (i = 0; i < *(int*)(c + 0x4668); i++) {
                        o = (i >= 13) ? 0 : ((char**)(c + 0x4688))[i];
                        if (*(unsigned char*)(o + 0x30) != 0
                            && func_ov006_02111dcc((i >= 13) ? 0 : ((char**)(c + 0x4688))[i], 1) != 0) {
                            func_ov006_02111e48((i >= 13) ? 0 : ((char**)(c + 0x4688))[i]);
                            func_ov006_0211470c(vec, (i >= 13) ? 0 : ((char**)(c + 0x4688))[i]);
                            func_ov006_02115598(c, vec, 0x7d0, 0, 1);
                            _ZN5Sound12PlayBank2_2DEj(0x175);
                            func_ov006_02114f98(c);
                            found = 1;
                            break;
                        }
                    }
                    if (found == 0) {
                        *(int*)(c + 0x5958) = 0;
                        *(unsigned char*)(c + 0x595c) = 0;
                    }
                }
            }
        }
        break;
    case 1:
        func_ov006_02115480(c);
        c[0xc3] = 0;
        if (*(int*)(c + 0x5960) == 0) {
            data_0209d454 |= 1;
        }
        if (*(int*)(c + 0x5998) > 0) {
            int* p = (int*)(((int)c + 0x5998) & 0xFFFFFFFFFFFFFFFF);
            *p -= 4;
        } else {
            int flag = 0;
            *(int*)(c + 0x5998) = 0;
            if (*(int*)(c + 0x5960) < 0x12c) {
                int* p = (int*)(((int)c + 0x5960) & 0xFFFFFFFFFFFFFFFF);
                (*p)++;
            } else {
                int idx = data_020a0e40;
                if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0) {
                    flag = 1;
                }
                if (flag != 0) {
                    if ((*(int*)(c + 8) & 0xff) == 0) {
                        func_020bc888 = 0x80;
                        func_020bc864 = -0x30;
                    }
                    func_ov004_020b0a54(0x10);
                    *(int*)(c + 0x4660) = 2;
                    func_02012790(0x62);
                }
            }
        }
        break;
    case 2:
        func_ov006_02115480(c);
        break;
    }

    if (*(int*)(c + 0x4784) > 0) {
        int* p = (int*)(((int)c + 0x4784) & 0xFFFFFFFFFFFFFFFF);
        (*p)--;
    }
    if (*(int*)(c + 0x4788) > 0) {
        int* p = (int*)(((int)c + 0x4788) & 0xFFFFFFFFFFFFFFFF);
        (*p)--;
    }

    func_ov006_02114c04(c);
    return 1;
}
