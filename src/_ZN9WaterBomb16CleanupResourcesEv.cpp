//cpp
extern "C" {
struct SharedFilePtr { unsigned short fileID; unsigned char numRefs; char* filePtr; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr* self);
extern struct SharedFilePtr data_ov002_0210da38;
extern struct SharedFilePtr data_ov098_0213c91c;

int _ZN9WaterBomb16CleanupResourcesEv(char* c){
  if (*(int*)(c+0x3c8) == 2) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da38);
  } else {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da38);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov098_0213c91c);
  }
  return 1;
}
}
