struct Pair { int a; int b; };

int func_0202044c(char* this, int idx)
{
    if (idx >= *(int*)(this + 0x10)) return 0;
    *(int*)(this + 0x14) = idx;
    *(int*)(this + 4) = ((struct Pair*)*(char**)this)[idx].a;
    *(int*)(this + 0x18) = ((struct Pair*)*(char**)this)[idx].b;
    *(int*)(this + 0x1c) = 0;
    return 1;
}
