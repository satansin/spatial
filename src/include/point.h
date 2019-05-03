struct Pt2D
{
	double x;
	double y;
};

struct Pt3D
{
	double x;
	double y;
	double z;
};

void print_pt(Pt2D* p);
void print_pt(Pt3D* p);

double eucl_dist(Pt2D p, Pt2D q);
double eucl_dist(Pt3D p, Pt3D q);

Pt3D sum_pt(Pt3D a, Pt3D b);

Pt3D scale_pt(double scalar, Pt3D p);

double dot_prd(Pt3D a, Pt3D b);

Pt3D cross_prd(Pt3D a, Pt3D b);

double sq_mode(Pt3D p);

double mode(Pt3D p);