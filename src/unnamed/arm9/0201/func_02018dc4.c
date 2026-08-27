extern char data_0208ec74;
struct BC { int x, y; };
int func_0205d568(int a, struct BC bc);
void func_02018e68(void *msg);
void Crash(void);

int func_02018dc4(int a, int b, int c, ...) {
    int r = func_0205d568(a, *(struct BC *)&b);
    if (r == 0) {
        func_02018e68(&data_0208ec74);
        Crash();
    }
    return r;
}
