typedef short s16;

extern void func_ov007_020cb7c0(int a, int b, int c, int d, int e, int f);
extern void func_ov007_020cb4b0(char* p, int a, int b, int c, int x, int y);
extern void func_ov007_020c368c(void* p, int n, int k);

/* Reconstructed layout and names, not recovered class identity; only the
 * slots this file touches. self is data_ov007_02104bd4[0], the 0x38-byte
 * grid mesh func_ov007_020cbbb0 builds (names as in func_ov007_020ca5f0). */
typedef struct Ov007Normal {
    s16 x, y, z;
} Ov007Normal;

typedef struct Ov007Mesh {
    void *verts;       /* 0x00: Vector3 per grid point (func_ov007_020cbbb0) */
    int *grid;         /* 0x04: func_ov007_020c73f0's grid; [0]/[1] are the point columns/rows */
    char pad_08[0x1c]; /* 0x08 */
    s16 cellWidth;     /* 0x24: func_ov007_020cbbb0's column spacing */
    s16 cellHeight;    /* 0x26: row spacing */
    void *faceNormals; /* 0x28: s16 triple per triangle, two per cell (func_ov007_020cb7c0) */
    Ov007Normal *vertexNormals; /* 0x2c: one per point (func_ov007_020cb4b0) */
} Ov007Mesh;

typedef char Ov007Mesh_size_must_be_0x30[sizeof(Ov007Mesh) == 0x30 ? 1 : -1];

/* Recomputes the mesh's normals: the face normals of every cell
 * (func_ov007_020cb7c0), then each point's normal from the faces around it
 * (func_ov007_020cb4b0), and flushes the point normals with
 * func_ov007_020c368c. self keeps the char * spelling of the definition;
 * `mesh` is the typed view. */
void func_ov007_020cb3dc(char* self)
{
    Ov007Mesh *mesh = (Ov007Mesh *)self;
    int x, y;
    int *t = mesh->grid;
    int w1 = t[0] - 1;
    int h1 = t[1] - 1;
    Ov007Normal *p;

    func_ov007_020cb7c0((int)mesh->faceNormals, w1, h1, (int)mesh->verts, mesh->cellWidth,
                        mesh->cellHeight);

    p = mesh->vertexNormals;
    for (y = 0; y < mesh->grid[1]; y++) {
        for (x = 0; x < mesh->grid[0]; x++) {
            func_ov007_020cb4b0((char *)p, (int)mesh->faceNormals, w1, h1, x, y);
            p++;
        }
    }

    func_ov007_020c368c(mesh->vertexNormals, mesh->grid[0] * mesh->grid[1], 1);
}
