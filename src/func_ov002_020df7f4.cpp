//cpp
struct State {};
struct Player { int IsState(State &); };
extern State data_ov002_02110244;

extern "C" int func_ov002_020df7f4(Player *self) {
    if (self->IsState(data_ov002_02110244)) {
        if (*(int *)((char *)self + 0x358) != 0)
            goto tail;
    }
    return -1;
tail:
    return *(unsigned char *)((char *)self + 0x6e3) ? 1 : 0;
}
