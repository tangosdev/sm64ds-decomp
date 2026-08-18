//cpp
typedef short s16;

struct dActor_c;

struct Camera {
    struct State;
    void LookAtExit(struct dActor_c &actor);
    void ChangeState(State *st);
};

extern Camera::State data_0209b0f8;

void Camera::LookAtExit(struct dActor_c &actor)
{
    int *src;
    char *a = (char *)&actor;

    *(int **)((char *)this + 0x11c) = (int *)(a + 0x5c);
    src = *(int **)((char *)this + 0x11c);
    *(int *)((char *)this + 0x120) = src[0];
    *(int *)((char *)this + 0x124) = src[1];
    *(int *)((char *)this + 0x128) = src[2];
    *(s16 *)((char *)this + 0x186) = -*(s16 *)(a + 0x8e);
    ChangeState(&data_0209b0f8);
    {
        int *fp = (int *)(((int)this + 0x154));
        *fp |= 0x10;
    }
}
