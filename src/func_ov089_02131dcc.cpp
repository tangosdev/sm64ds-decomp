//cpp
class Event {
public:
static void SetBit(unsigned int);
};

class ActorBase {
public:
void MarkForDestruction();
};

extern "C" void func_ov002_020c3dbc(int);

extern "C" void func_ov089_02131dcc(ActorBase *r4, int r1) {
func_ov002_020c3dbc(r1);
Event::SetBit(0x1d);
r4->MarkForDestruction();
}
