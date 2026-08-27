//cpp
struct SharedFilePtr { void Release(); };
struct Model { ~Model(); };
extern SharedFilePtr data_ov006_02142f80;
extern "C" Model* func_ov006_02122c68(Model* m){
  data_ov006_02142f80.Release();
  m->~Model();
  return m;
}
