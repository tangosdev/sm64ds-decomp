extern void func_ov007_020c7240(void* a, int b, int c);
extern void func_ov007_020c7a84(void* a, int b, int c);
extern void func_ov007_020c71c0(void* a, int b, int c);

/* data_ov007_02104bd4[0], the 0x38-byte grid mesh func_ov007_020cbbb0
 * builds; names as in func_ov007_020ca5f0 and func_ov007_020cbb04, only the
 * slots this file reads. */
typedef struct Ov007Mesh {
    char pad_00[4];
    int *grid; /* 0x04: func_ov007_020c73f0's grid; [0]/[1] are the point columns/rows */
    /* 0x08: one func_ov007_020c80a4 link per cell, (columns - 1) * (rows - 1)
     * of them (func_ov007_020cbbb0) */
    void **cellLinks;
    /* 0x0c..0x23: func_ov007_020cbbb0 copies its 0x18-byte template here */
    char pad_0c[4];
    /* 0x10/0x14: the mesh's spring, for the grid's rows and columns
     * (func_ov007_020c7240) and for each cell link's z (func_ov007_020c7a84) */
    int stiffness;
    int damping;
} Ov007Mesh;

typedef char Ov007Mesh_size_must_be_0x18[sizeof(Ov007Mesh) == 0x18 ? 1 : -1];

/* One spring step: the grid's rows and columns, then every cell link along
 * z, then the points integrate (func_ov007_020c71c0). c keeps its char *
 * spelling; `mesh` is the typed view. */
void func_ov007_020cba78(char* c, int arg1) {
    Ov007Mesh *mesh = (Ov007Mesh *)c;
    int i;
    int n;
    func_ov007_020c7240(mesh->grid, mesh->stiffness, mesh->damping);
    for (i = 0; (n = (mesh->grid[1] - 1) * (mesh->grid[0] - 1)), i < n; i++) {
        func_ov007_020c7a84(mesh->cellLinks[i], mesh->stiffness, mesh->damping);
    }
    func_ov007_020c71c0(mesh->grid, 0, arg1);
}
