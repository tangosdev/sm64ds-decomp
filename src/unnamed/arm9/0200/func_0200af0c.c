extern int data_0208715c;

struct S {
    char _pad[0x13c];
    int *field_13c;
};

int func_0200af0c(struct S *p) {
    p->field_13c = &data_0208715c;
    return 1;
}
