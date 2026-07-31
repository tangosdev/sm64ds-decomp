//cpp
struct MeshColliderBase {
    int IsEnabled();
};

int MeshColliderBase::IsEnabled()
{
    char * c = (char *)this;
unsigned char v = c[0x14];
  if (v != 0x18) return 1;
  return 0;

}
