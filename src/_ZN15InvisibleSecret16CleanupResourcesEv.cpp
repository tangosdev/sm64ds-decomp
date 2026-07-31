//cpp
extern "C" {
extern int data_ov002_0210da28[];
extern int data_ov002_0210da08[];
extern int data_ov002_0210d9a8[];
extern int data_ov002_0210d9e8[];
void _ZN13SharedFilePtr7ReleaseEv(void*);
}

struct InvisibleSecret {
    int CleanupResources();
};

int InvisibleSecret::CleanupResources()
{
    char* c = (char*)this;
if(*(int*)(c+8) & 0x10){
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da28);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da08);
  } else {
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9e8);
  }
  return 1;

}
