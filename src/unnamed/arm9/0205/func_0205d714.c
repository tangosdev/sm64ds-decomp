typedef struct {
    void *field_0;
    short field_4;
    short field_6;
    int field_8;
} Tail;

typedef struct {
    char pad0[0x10];
    int field_10;
} Obj;

typedef struct {
    char pad0[8];
    int field_8;
    char pad_c[0x2c - 0xc];
    Tail field_2c;
    char *field_38;
    int field_3c;
    int field_40;
} Out;

extern Tail data_020a804c;
extern Obj *func_0205d23c(char *s, int i);
extern int func_0205cdf4(Out *out, int n);

int func_0205d714(Out *out, char *path, int a2, int a3)
{
    Tail t;
    int i;
    Obj *obj;
    int ok;
    unsigned char c;

    c = (unsigned char)path[0];
    if (c == '/' || c == '\\') {
        t.field_4 = 0;
        t.field_0 = data_020a804c.field_0;
        t.field_8 = 0;
        t.field_6 = 0;
        path++;
    } else {
        t = data_020a804c;
        for (i = 0; i <= 3; i++) {
            c = (unsigned char)path[i];
            if (c == 0 || c == '/' || c == '\\')
                break;
            if (c == ':') {
                obj = func_0205d23c(path, i);
                if (obj == 0)
                    return 0;
                ok = (obj->field_10 & 2) ? 1 : 0;
                if (ok == 0)
                    return 0;
                t.field_0 = obj;
                t.field_8 = 0;
                t.field_6 = 0;
                t.field_4 = 0;
                path += i + 1;
                c = (unsigned char)*path;
                if (c == '/' || c == '\\')
                    path++;
                break;
            }
        }
    }

    out->field_8 = (int)t.field_0;
    out->field_38 = path;
    out->field_2c = t;
    if (a3 != 0) {
        out->field_3c = 1;
        out->field_40 = a3;
    } else {
        out->field_3c = 0;
        out->field_40 = a2;
    }
    return func_0205cdf4(out, 4);
}
