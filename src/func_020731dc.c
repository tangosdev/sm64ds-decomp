extern void *data_020aa3f0;
void func_020731dc(void *object, void *destructor, void **node)
{
    node[0] = data_020aa3f0;
    node[1] = destructor;
    node[2] = object;
    data_020aa3f0 = node;
}
