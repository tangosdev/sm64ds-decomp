//cpp
struct State;
struct Player {
    void St_Shell_Cleanup();
    void ChangeState(State &);
};
extern State data_ov002_0211013c;

extern "C" void func_ov002_020cc1f4(Player *p) {
    if (*(int *)((char *)p + 0x354) == 0) return;
    p->St_Shell_Cleanup();
    p->ChangeState(data_ov002_0211013c);
}
