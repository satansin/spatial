#include "point.h"

struct Ratio_set {
	double inradius;
	double circumradius;
	double ratio;
};

Pt2D circumcenter_2d(Pt2D a, Pt2D b, Pt2D c);
Pt3D circumcenter_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d);

Ratio_set get_ratio_set_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d);

double circumradi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d);

double inradi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d);

double bounding_radi_3d(Pt3D a, Pt3D b, Pt3D c, Pt3D d);