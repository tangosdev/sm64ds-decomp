struct Vec3
{
  int x;
  int y;
  int z;
};
void func_ov007_020c2f14(struct Vec3 *points, int count, int *minIdxX, int *maxIdxX, int *minIdxY, int *maxIdxY, int *minIdxZ, int *maxIdxZ)
{
  struct Vec3 mx;
  struct Vec3 mn;
  struct Vec3 tmp;
  int i;
  int miX;
  int mxX;
  int miY;
  int mxY;
  int miZ;
  int mxZ;
  tmp = points[0];
  mx = tmp;
  mn = tmp;
  mxZ = 0;
  miZ = 0;
  mxY = 0;
  miY = 0;
  mxX = 0;
  miX = 0;
  for (i = 1; i < count; i++)
  {
    if (points[i].x < mn.x)
    {
      miX = i;
      mn.x = points[i].x;
    }
    else
      if (points[i].x > mx.x)
    {
      mxX = i;
      mx.x = points[i].x;
    }
    if (points[i].y < mn.y)
    {
      miY = i;
      mn.y = points[i].y;
    }
    else
      if (points[i].y > mx.y)
    {
      mxY = i;
      mx.y = points[i].y;
    }
    if (points[i].z < mn.z)
    {
      miZ = i;
      mn.z = points[i].z;
    }
    else
      if (points[i].z > mx.z)
    {
      mxZ = i;
      mx.z = points[i].z;
    }
  }

  if (minIdxX)
  {
    *minIdxX = miX;
  }
  if (minIdxY)
  {
    *minIdxY = miY;
  }
  if (minIdxZ)
  {
    *minIdxZ = miZ;
  }
  if (maxIdxX)
  {
    *maxIdxX = mxX;
  }
  if (maxIdxY)
  {
    *maxIdxY = mxY;
  }
  if (maxIdxZ)
  {
    *maxIdxZ = mxZ;
  }
}
