//cpp
extern "C" {
struct SharedFilePtr { unsigned short fileID; unsigned char numRefs; char* filePtr; };
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr* self);
extern struct SharedFilePtr BUBBLE_MODEL_PTR;
extern struct SharedFilePtr data_ov098_0213c91c;

int func_ov098_0213bb9c(char* c){
  if (*(int*)(c+0x3c8) == 2) {
    _ZN13SharedFilePtr7ReleaseEv(&BUBBLE_MODEL_PTR);
  } else {
    _ZN13SharedFilePtr7ReleaseEv(&BUBBLE_MODEL_PTR);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov098_0213c91c);
  }
  return 1;
}
}
