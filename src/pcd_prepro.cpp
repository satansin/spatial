#include "TriMesh.h"
#include <iostream>
using namespace std;
using namespace trimesh;

int main(int argc, char *argv[])
{
    const char *filename = "../bun_zipper_res4.ply";
    TriMesh *m = TriMesh::read(filename);
    if (!m)
        exit(1);

    cout << "There are " << m->vertices.size() << " vertices" << endl;
    cout << "Vertex 0 is at " << m->vertices[0] << endl;

    // Convert triangle strips to faces, if necessary
    m->need_faces();
    cout << "Face 0 has vertices " << m->faces[0][0] << ", "
         << m->faces[0][1] << ", and " << m->faces[0][2] << endl;

    m->need_normals();
    cout << "Vertex 0 has normal " << m->normals[0] << endl;
}