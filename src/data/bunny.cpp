#include "TriMesh.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace trimesh;

double d(point p, point q) {
	return sqrt((p[0] - q[0]) * (p[0] - q[0]) + (p[1] - q[1]) * (p[1] - q[1]) + (p[2] - q[2]) * (p[2] - q[2]));
}

int main() {
	TriMesh *bunny = TriMesh::read("t1.ply");
	// bunny->need_bsphere();
	// cout << bunny->bsphere.r << endl;
	double sum = 0;
	for (TriMesh::Face &i: bunny->faces) {
		point v1 = bunny->vertices[i[0]];
		point v2 = bunny->vertices[i[1]];
		point v3 = bunny->vertices[i[2]];
		
		sum += d(v1, v2);
		sum += d(v2, v3);
		sum += d(v3, v1);
	}

	double avg = sum / (3 * bunny->faces.size());
	cout << avg << endl;
}