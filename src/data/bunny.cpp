#include "TriMesh.h"
#include <iostream>

using namespace std;
using namespace trimesh;

int main() {
	TriMesh *bunny = TriMesh::read("t1.ply");
	bunny->need_bsphere();
	cout << bunny->bsphere.r << endl;
}