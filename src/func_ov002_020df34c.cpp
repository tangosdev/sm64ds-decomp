//cpp
struct State {};
struct Camera {};
struct Player { int IsState(State& s); void ChangeState(State& s); };
extern "C" int func_ov002_020e0478(void* c);
extern "C" void func_0200d474(Camera* thiz, unsigned char playerID);
extern State _ZN6Player13ST_LEDGE_HANGE;
extern State _ZN6Player9ST_CANNONE;
extern Camera* CAMERA;
extern "C" int func_ov002_020df34c(Player* self) {
    unsigned char* p = (unsigned char*)self;
    if (p[0x709] != 0 || func_ov002_020e0478(self) != 0 || self->IsState(_ZN6Player13ST_LEDGE_HANGE) != 0) {
        return 0;
    }
    p[0x6e3] = 0;
    self->ChangeState(_ZN6Player9ST_CANNONE);
    func_0200d474(CAMERA, p[0x6d8]);
    return 1;
}
