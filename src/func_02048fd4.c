// Copies pair i (i in 0..8) from table data_020821c0 into
// data_02099fa8 / data_02099fac.
typedef unsigned int u32;

extern u32 data_020821c0[];
extern volatile u32 data_02099fa8;
extern volatile u32 data_02099fac;

void func_02048fd4(int i)
{
    volatile u32 *tbl;
    int j;
    if (i < 0) return;
    if (i > 8) return;
    tbl = data_020821c0;
    j = i * 2;
    data_02099fa8 = tbl[j];
    j++;
    data_02099fac = tbl[j];
}
