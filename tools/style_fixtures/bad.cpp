//cpp
// Intentionally rejected by pattern tests; this fixture is never linked to the ROM.
void FixtureBad(void* self)
{
    *(int*)(self + 0x74) = 1;
    ((char*)self + 0x80)[0] = 0;
    actor->vtable[3](actor);
    _ZN5Actor4TickEv(self);
    actor->unk_74 = 0;
    actor = nullptr;
}
