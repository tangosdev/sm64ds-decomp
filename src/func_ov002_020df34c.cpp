//cpp
struct State {};
struct Camera {};
struct Player { int IsState(State& s); void ChangeState(State& s); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN6Player7IsStateERNS_5StateE(void *, State& s);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *, State& s);

extern "C" int func_ov002_020e0478(void* c);
extern "C" void func_0200d474(Camera* thiz, unsigned char playerID);
extern State data_ov002_0210ffec;
extern State data_ov002_021102d4;
extern Camera* data_0209f318;
extern "C" int func_ov002_020df34c(Player* self) {
    unsigned char* p = (unsigned char*)self;
    if (p[0x709] != 0 || func_ov002_020e0478(self) != 0 || _ZN6Player7IsStateERNS_5StateE(self, data_ov002_0210ffec) != 0) {
        return 0;
    }
    p[0x6e3] = 0;
    _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_021102d4);
    func_0200d474(data_0209f318, p[0x6d8]);
    return 1;
}
