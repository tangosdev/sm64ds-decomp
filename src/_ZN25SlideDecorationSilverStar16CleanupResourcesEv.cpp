//cpp
class SharedFilePtr {
public:
void Release();
};

extern "C" int data_ov031_02111424[];

extern "C" int _ZN25SlideDecorationSilverStar16CleanupResourcesEv(char *r0) {
unsigned char r1 = *(unsigned char *)(r0 + 0x124);
SharedFilePtr *ptr = (SharedFilePtr *)data_ov031_02111424[r1];
ptr->Release();
return 1;
}
