//cpp
// @symbol _ZN8dActor_c4NextEPKS_
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
struct dActor_c;
extern struct dActor_c* data_0209b468;   /* the global actor list; head link at +0 */

/* Walks the global actor list. A null receiver means "start at the head".
   The list threads through each actor's mListPrev/mListNext/mListOwner block at
   0x50, so a node IS another actor's mListPrev and its owner sits at node + 8. */
struct dActor_c* _ZN8dActor_c4NextEPKS_(struct dActor_c *self) {
  void* node;
  if (self) node = self->mListNext;
  else      node = *(void**)&data_0209b468;
  if (node) return *(struct dActor_c**)((char*)node + 8);
  return 0;
}
}
