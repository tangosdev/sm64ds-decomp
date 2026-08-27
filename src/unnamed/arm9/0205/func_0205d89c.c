extern int data_020a8058;
extern void func_0205d96c(int);

void func_0205d89c(int x) {
    if (!data_020a8058) {
        data_020a8058 = 1;
        func_0205d96c(x);
    }
}
