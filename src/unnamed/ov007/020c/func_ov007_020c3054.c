void func_ov007_020c3054(int* arr, int n, int* pmin, int* pmax) {
    int i;
    int maxv = (int)0x80000000;
    int minv = 0x7fffffff;
    for (i = 0; i < n; i++) {
        int v = arr[i];
        if (v < minv) { *pmin = i; minv = v; }
        if (v > maxv) { *pmax = i; maxv = v; }
    }
}
