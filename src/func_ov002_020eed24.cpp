//cpp
extern "C" int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern "C" int func_02035638(void*);
extern "C" void* func_0203567c(void*);
extern "C" int _ZNK5dBgPi9GetClsnIDEv(void*);

struct dActor_c {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26();
    virtual void m(void* arg);  /* slot 27 = 0x6c */
};

extern "C" dActor_c* _ZN8dActor_c10FindWithIDEj(unsigned int id);

extern "C" int func_ov002_020eed24(void* c, void* arg){
    void* r;
    dActor_c* a;
    if (_ZNK10dBgCh_Actr8IsOnWallEv(c)
        || _ZNK10dBgCh_Actr10IsOnGroundEv(c)
        || func_02035638(c)) {
        r = func_0203567c(c);
        if (_ZNK5dBgPi9GetClsnIDEv(r) != -1) {
            a = _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(r));
            if (a) {
                a->m(arg);
                return 1;
            }
        }
    }
    return 0;
}
