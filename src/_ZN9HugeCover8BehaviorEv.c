extern void _ZN9Animation7AdvanceEv(void *);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *);
int _ZN9HugeCover8BehaviorEv(char *c)
{
    *(int *)(c + 0x32c) = 4096;
    _ZN9Animation7AdvanceEv((char *)c + 0x320);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
