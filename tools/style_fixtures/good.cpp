//cpp
// Explanatory references such as _ZN5Actor4TickEv or obj->vtable[0] are comments.
typedef unsigned int u32;

struct FixtureActor {
    FixtureActor() : timer(0) {}
    void Tick();
    int timer;
};

void FixtureActor::Tick() {
    if (timer) {
        --timer;
    }
}

void SetFixtureRegister() {
    *reinterpret_cast<volatile u32 *>(0x04000208) = 0;
}
