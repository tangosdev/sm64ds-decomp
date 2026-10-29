//cpp
extern "C" void func_ov007_020c8098(void* p);
extern "C" void func_ov007_020c7368(void* s);
extern "C" void func_ov007_020c3d1c(void* self);

/* data_ov007_02104bd4[0], the 0x38-byte grid mesh func_ov007_020cbbb0
 * builds; names as in func_ov007_020ca5f0, only the slots this file frees. */
struct Ov007Mesh {
    void *verts; /* 0x00: Vector3 per grid point (func_ov007_020cbbb0) */
    int *grid;   /* 0x04: func_ov007_020c73f0's grid; [0]/[1] are the point columns/rows */
    /* 0x08: one func_ov007_020c80a4 link per cell, (columns - 1) * (rows - 1)
     * of them (func_ov007_020cbbb0) */
    void **cellLinks;
    char pad_0c[0x1c];
    void *faceNormals;   /* 0x28: s16 triple per triangle, two per cell (func_ov007_020cb7c0) */
    void *vertexNormals; /* 0x2c: s16 triple per point (func_ov007_020cb4b0) */
    void *texOffsetS;    /* 0x30: func_ov007_020caeac's per-column tu scratch */
    void *texOffsetT;    /* 0x34: its tv scratch */
};

typedef char Ov007Mesh_size_must_be_0x38[sizeof(Ov007Mesh) == 0x38 ? 1 : -1];

/* Frees the mesh: its buffers, every cell link (func_ov007_020c8098), the
 * link table, the grid (func_ov007_020c7368) and the mesh itself. c keeps
 * its char * spelling; `mesh` is the typed view. */
extern "C" void func_ov007_020cbb04(char* c)
{
    Ov007Mesh *mesh = (Ov007Mesh *)c;
    func_ov007_020c3d1c(mesh->verts);
    func_ov007_020c3d1c(mesh->faceNormals);
    func_ov007_020c3d1c(mesh->vertexNormals);
    func_ov007_020c3d1c(mesh->texOffsetS);
    func_ov007_020c3d1c(mesh->texOffsetT);

    int i = 0;
    int *a = mesh->grid;
    if ((a[1] - 1) * (a[0] - 1) > 0) {
        do {
            func_ov007_020c8098(mesh->cellLinks[i]);
            i++;
            a = mesh->grid;
        } while (i < (a[1] - 1) * (a[0] - 1));
    }
    func_ov007_020c3d1c(mesh->cellLinks);
    func_ov007_020c7368(mesh->grid);
    func_ov007_020c3d1c((void*)c);
}
