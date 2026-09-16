//cpp
#include "Model.h"
extern "C" {
struct SharedFilePtr{int x;};
extern SharedFilePtr* data_ov089_02132894;
extern SharedFilePtr* data_ov089_021328b4;
void LoadKeyModels(int idx){
  if(idx>=8) return;
  Model::LoadFile(*(SharedFilePtr*)((&data_ov089_02132894)[idx]));
  SharedFilePtr* p=(&data_ov089_021328b4)[idx];
  if(p==0) return;
  Model::LoadFile(*p);
}
}
