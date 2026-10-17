/* fBase_c's scene-tree node (include/fBase_c.h, fBase_c::SceneNode at
 * +0x14) and the one owner byte read here, under the header's names. The
 * walk visits every descendant: func_0203b394 steps to the first child or,
 * failing that, func_0203b358's next node outside the subtree, which is
 * also where the walk ends. It reports whether any descendant's aliveState
 * is still 0; d_scene.cpp waits on that. */
struct fBaseView;
struct Node {
    struct Node *parent;
    struct Node *child; /* 0x04 */
    struct Node *prev;
    struct Node *next;
    struct fBaseView *owner; /* 0x10 */
};
struct fBaseView {
    unsigned char pad_00[0xe];
    unsigned char aliveState; /* 0x0e */
    unsigned char pad_0f[5];
    struct Node sceneNode; /* 0x14 */
};
extern void *func_0203b358(struct Node *thiz);
extern void *func_0203b394(struct Node *thiz);
int func_020431c4(void *thiz) {
    struct Node *base = &((struct fBaseView *)thiz)->sceneNode;
    void *end = func_0203b358(base);
    struct Node *node = base->child;
    int t;
    while (node != 0 && (void *)node != end) {
        t = !node->owner->aliveState;
        if (t != 0)
            return 1;
        node = (struct Node *)func_0203b394(node);
    }
    return 0;
}
