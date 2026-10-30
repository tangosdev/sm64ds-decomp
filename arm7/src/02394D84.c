extern int fn_02380884(int key);
extern void fn_023808C0(int token);
extern int fn_02394B88(void *list, unsigned short *node);
extern int fn_02394C70(void *list, unsigned short *node);

int fn_02394D84(void *a1, void *a2, unsigned short *node) {
    int rc;
    int token;

    if (node[5] != 0xBF1D)
        return 1;
    token = fn_02380884(0x1000000);
    rc = fn_02394B88(a1, node);
    if (rc == 0)
        rc = fn_02394C70(a2, node);
    fn_023808C0(token);
    return rc;
}
