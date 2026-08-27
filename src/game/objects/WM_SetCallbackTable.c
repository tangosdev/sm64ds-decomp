extern int *data_020a89ac;
void WM_SetCallbackTable(int i, int val) {
    int *arr = data_020a89ac;
    (arr + i)[5] = val;
}
