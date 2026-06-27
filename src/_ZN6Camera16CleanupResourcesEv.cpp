//cpp
extern "C" {
    extern void _ZN6Memory16operator_delete2EPv(void *);
    int _ZN6Camera16CleanupResourcesEv(void *thiz) {
        int p = *(int *)((char *)thiz + 0x148);
        if (p != 0 && p != 0) {
            _ZN6Memory16operator_delete2EPv((void *)p);
        }
        return 1;
    }
}
