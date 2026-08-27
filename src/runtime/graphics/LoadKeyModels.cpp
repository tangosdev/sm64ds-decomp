//cpp
extern "C" {
struct SharedFilePtr{int x;};
extern SharedFilePtr* data_ov089_02132894;
extern SharedFilePtr* data_ov089_021328b4;
void _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
void LoadKeyModels(int idx){
  if(idx>=8) return;
  _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr*)((&data_ov089_02132894)[idx]));
  SharedFilePtr* p=(&data_ov089_021328b4)[idx];
  if(p==0) return;
  _ZN5Model8LoadFileER13SharedFilePtr(*p);
}
}
