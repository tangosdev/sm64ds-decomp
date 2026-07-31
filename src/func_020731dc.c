extern void *data_020aa3f0;
void func_020731dc(int a, int b, void **node)
{
    node[0] = data_020aa3f0;
    node[1] = (void *)b;
    node[2] = (void *)a;
    data_020aa3f0 = node;
}
