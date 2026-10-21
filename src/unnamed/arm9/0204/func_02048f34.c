extern void func_0204f94c(void *p);
extern void func_02048fd4(int x);

typedef struct { int a, b; } Elem;

extern int *data_020a4bec;
extern Elem data_020a4c18[];
static Elem data_020a4bf0;
static Elem data_020a4bf8[2];
static Elem data_020a4c08[2];

void func_02048f34(int *owner)
{
    int i;
    data_020a4bec = owner;
    for (i = 0; i < 6; i++) func_0204f94c(&data_020a4c18[i]);
    for (i = 0; i < 2; i++) func_0204f94c(&data_020a4c08[i]);
    for (i = 0; i < 2; i++) func_0204f94c(&data_020a4bf8[i]);
    func_0204f94c(&data_020a4bf0);
    func_02048fd4(0);
}
