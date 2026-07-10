//cpp
extern "C" {
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(char* p);
}

extern "C" void _ZN5Model11UpdateVertsEv(char* self)
{
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(self + 8);
}
