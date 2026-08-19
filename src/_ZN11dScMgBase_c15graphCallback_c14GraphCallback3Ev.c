int _ZN11dScMgBase_c15graphCallback_c14GraphCallback3Ev(char *c) {
    char *p = *(char**)(c+4);
    if (p != 0) {
        void (*fn)(char*) = *(void(**)(char*))( (char*)(*(void**)p) + 0x64 );
        fn(p);
    }
    return 1;
}
