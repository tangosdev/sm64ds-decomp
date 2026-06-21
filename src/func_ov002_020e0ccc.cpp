//cpp
struct State { char pad[0xc]; unsigned short id; };
struct Player {
    void ChangeState(State &);
};
extern State data_ov002_02110484;

extern "C" int func_ov002_020e0ccc(Player *self, State *s) {
    if (s == 0) return 0;
    int ok = s->id == 0xc1;
    if (!ok) return 0;
    self->ChangeState(data_ov002_02110484);
    return 1;
}
