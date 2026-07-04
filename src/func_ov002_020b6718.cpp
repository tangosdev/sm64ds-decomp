//cpp
extern "C" {
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void*, int, int);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
int func_ov002_020b6718(char* c) {
    short* p=(short*)(((long long)(int)(c+0x94))&0xFFFFFFFFFFFFFFFFLL);
    *p = *p + *(short*)(c+0x96);
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
}
