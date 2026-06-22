typedef struct Node87 {
    char pad4[4];
    int field_4;
    int field_8;
} Node87;

typedef struct Obj87 {
    char pad7c[0x7c];
    int field_7c;
} Obj87;

extern Node87 *func_02058960(Obj87 *o);
extern void func_0205807c(void);

void func_02058a44(Obj87 *o)
{
    Node87 *n;
    if (o->field_7c == 0)
        return;
    do {
        n = func_02058960(o);
        n->field_8 = 0;
        n->field_4 = 0;
        func_0205807c();
    } while (o->field_7c != 0);
}
