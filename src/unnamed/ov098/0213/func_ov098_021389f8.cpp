//cpp
// func_ov098_021389f8 at 0x021389f8
extern "C" {
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
void dBgCh_Actr_UpdateContinuous_Veneer(void *);
void func_ov098_02139228(void *);
void func_ov098_02138e6c(void *);
void func_ov098_021390ec(void *);
int func_ov098_02138bb8(void *);
int func_ov098_02138bfc(void *);
void _ZN5dCc_c5ClearEv(void *);
void _ZN5dCc_c6UpdateEv(void *);
void func_ov098_02139850(void *);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void *);
void func_ov098_021396a4(void *);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *, int, int);
void func_ov098_021397c8(void *);
}

struct Obj {
    virtual void v0();   virtual void v1();   virtual void v2();   virtual void v3();
    virtual void v4();   virtual void v5();   virtual void v6();   virtual void v7();
    virtual void v8();   virtual void v9();   virtual void v10();  virtual void v11();
    virtual void v12();  virtual void v13();  virtual void v14();  virtual void v15();
    virtual void v16();  virtual void v17();  virtual void v18();  virtual void v19();
    virtual void v20();  virtual void v21();  virtual void v22();  virtual void v23();
    virtual void v24();  virtual void v25();  virtual void v26();  virtual void v27();
    virtual void v28();  virtual void v29();  virtual void v30();  virtual void v31();
};

extern "C" void func_ov098_021389f8(char *c);
void func_ov098_021389f8(char *c) {
    int flag = (*(int *)(c + 0xb0) & 8) != 0;
    if (flag) {
        if (*(int *)(c + 0x98) == 0) {
            if (*(int *)(c + 0xa8) == 0) {
                return;
            }
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x564);
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x320);
    func_ov098_02139228(c);
    func_ov098_02138e6c(c);
    func_ov098_021390ec(c);
    if (func_ov098_02138bb8(c) || func_ov098_02138bfc(c)) {
        ((Obj *)c)->v31();
    }
    _ZN5dCc_c5ClearEv(c + 0x564);
    _ZN5dCc_c6UpdateEv(c + 0x564);
    _ZN5dCc_c5ClearEv(c + 0x5a4);
    _ZN5dCc_c6UpdateEv(c + 0x5a4);
    func_ov098_02139850(c);
    if (!_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x320)) {
        func_ov098_021396a4(c);
    }
    if (!_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x600000, 0)) {
        return;
    }
    func_ov098_021397c8(c);
}
