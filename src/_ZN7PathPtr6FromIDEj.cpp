//cpp
struct PathPtr { int a; int b; };
extern "C" {
extern int data_020a0d84;
PathPtr func_0203ad6c(int v);

PathPtr _ZN7PathPtr6FromIDEj(unsigned int id) {
    return func_0203ad6c(data_020a0d84 + id * 6);
}
}
