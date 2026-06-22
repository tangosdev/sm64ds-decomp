//cpp
// NONMATCHING: base materialization / addressing (div=4). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Base { virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int v5(); virtual int v6(); virtual int v7(); virtual int v8(); virtual int v9(); virtual int v10(); virtual int v11(); virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15(); virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19(); virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23(); virtual int v24(); virtual int v25(); virtual int v26(); virtual int v27(); virtual int v28(); virtual int m74(); };
struct Vec { int x, y, z; };
extern "C" int _ZN5Actor25OnAimedAtWithEggReturnVecEv(Vec *self, Base *arg)
{
    char *a = (char*)arg;
    self->x = *(int*)(a+0x5c);
    self->y = *(int*)(a+0x60);
    self->z = *(int*)(a+0x64);
    int r = arg->m74();
    self->y += r;
    return self->y;
}
