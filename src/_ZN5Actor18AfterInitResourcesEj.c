typedef unsigned int u32;
typedef struct ActorDerived { char pad[0]; } ActorDerived;

extern void _ZN12ActorDerived18AfterInitResourcesEj(ActorDerived *self, u32 result);

void _ZN5Actor18AfterInitResourcesEj(char *self, u32 result)
{
    _ZN12ActorDerived18AfterInitResourcesEj((ActorDerived *)self, result);
    *(int *)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x38;
}
