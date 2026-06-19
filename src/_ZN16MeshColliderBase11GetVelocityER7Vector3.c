struct V3 { int x, y, z; };
void _ZN16MeshColliderBase11GetVelocityER7Vector3(unsigned char *c, struct V3 *v) {
  v->z = 0;
  v->y = v->z;
  v->x = v->y;
}
