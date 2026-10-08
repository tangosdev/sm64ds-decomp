extern int func_02060470(void);
extern void func_02060f60(void *a, int b, int c);
extern void func_0206081c(void *a);

void func_020607e8(void *thiz)
{
    int **pp = (int **)thiz;
    (*pp)[1] = func_02060470();
    func_02060f60(thiz, 2, 1);
    func_0206081c(thiz);
}
