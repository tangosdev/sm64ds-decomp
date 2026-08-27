typedef int (*Fn)(void *);
extern Fn data_020a4b58;

int func_02043060(void *a)
{
    if (data_020a4b58 == 0)
        return 2;
    return data_020a4b58(a);
}
