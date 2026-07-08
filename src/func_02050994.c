extern int *_ZN5Sound8SDAT_RAM3PTRE;
void func_02050994(int i, int val) {
    int *p = (int*)_ZN5Sound8SDAT_RAM3PTRE[0x7c / 4];
    (p + i * 4)[5] = val;
}
