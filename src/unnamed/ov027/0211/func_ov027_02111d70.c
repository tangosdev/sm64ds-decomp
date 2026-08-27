typedef struct Elem {
    int a, b, c, d;
} Elem;

extern Elem data_ov027_02113ce4[];
extern void func_ov027_02111d38(int *self);

void func_ov027_02111d70(int *self, int idx)
{
    *(Elem **)((char *)self + 0x3cc) = &data_ov027_02113ce4[idx];
    func_ov027_02111d38(self);
}
