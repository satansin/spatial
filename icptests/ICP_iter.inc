/*
Szymon Rusinkiewicz
Princeton University

ICP_iter.inc
Routines for implementing an ICP iteration.
This needs to be combined with ICP_align.inc and a driver routine to get
a full ICP.
*/

#include <set>

#include "../testing_terrain/mathutil.h"

#include "TriMesh.h"
#include "XForm.h"
#include "KDtree.h"


namespace trimesh {

point nn_point(point q, TriMesh* mesh, vec& ret_fnormal) {
	double min_sq_dist = INFTY;
	point nn_pt;
	int nn_f_i;

	point fv[3];
	for (int i = 0; i < mesh->faces.size(); i++) {
		for (int j = 0; j < 3; j++) {
			fv[j] = mesh->vertices[mesh->faces[i][j]];
		}
		point tmp_res;
		int flag;
		double new_sq_dist = sq_dist_p2f(q, fv, tmp_res, flag);

		if (min_sq_dist > new_sq_dist) {
			min_sq_dist = new_sq_dist;
			nn_pt = tmp_res;
			nn_f_i = i;
		}
	}

	ret_fnormal = mesh->trinorm(nn_f_i);

	return nn_pt;
}


// A class for evaluating compatibility of normals during KDtree searches.
class NormCompat : public KDtree::CompatFunc {
private:
	const TriMesh *m;
	const vec &n;
	float thresh;

public:
	NormCompat(const vec &n_, const TriMesh *m_, float thresh_):
		m(m_), n(n_), thresh(thresh_)
		{}
	virtual bool operator () (const float *p) const
	{
		int idx = (const point *) p - &(m->vertices[0]);
		return (n DOT m->normals[idx]) > thresh;
	}
};


// Full sampling on mesh1, find closest on mesh2 according to variant, append to pairs
static void tri_select_and_match(TriMesh *mesh1, TriMesh *mesh2,
                            	 const xform &xf1, const xform &xf2,
                            	 const KDtree *kd2, float cdfincr,
                            	 float approx_eps, float maxdist,
                            	 float normdot_thresh, ICP_Variant variant,
                            	 bool do_flip, ::std::vector<PtPair> &pairs)
{
	size_t nv1 = mesh1->vertices.size();
	xform nxf1 = norm_xf(xf1);
	xform nxf2 = norm_xf(xf2);
	xform xf12 = inv(xf2) * xf1;
	xform nxf12 = norm_xf(xf12);
	float maxdist2 = sqr(maxdist);

	size_t nf1 = mesh1->faces.size();

	if (variant <= VARIANT_SYMM_LM) {
		for (size_t i = 0; i < nv1; i++) {
			// Transform into coords of mesh2 and match
			point p12 = xf12 * mesh1->vertices[i];

			const float *match;
			if (normdot_thresh >= 0.0f) {
				vec n12 = nxf12 * mesh1->normals[i];
				NormCompat nc(n12, mesh2, normdot_thresh);
				match = kd2->closest_to_pt(p12, maxdist2, &nc, approx_eps);
			} else {
				match = kd2->closest_to_pt(p12, maxdist2, approx_eps);
			}
			if (!match)
				continue;

			int match_ind = (const point *) match - &(mesh2->vertices[0]);

			// Project both points into world coords and save
			point p1 = xf1 * mesh1->vertices[i];
			vec n1 = nxf1 * mesh1->normals[i];
			point p2 = xf2 * mesh2->vertices[match_ind];
			vec n2 = nxf2 * mesh2->normals[match_ind];
			if ((n1 DOT n2) < 0.0f)
				continue;

			if (do_flip)
				pairs.push_back(PtPair(p2, n2, p1, n1));
			else
				pairs.push_back(PtPair(p1, n1, p2, n2));

			if (variant != VARIANT_MITRA)
				continue;

			if (do_flip) {
				pairs.back().pdir1 = nxf2 * mesh2->pdir1[match_ind];
				pairs.back().pdir2 = nxf2 * mesh2->pdir2[match_ind];
				pairs.back().curv1 = mesh2->curv1[match_ind];
				pairs.back().curv2 = mesh2->curv2[match_ind];
			} else {
				pairs.back().pdir1 = nxf1 * mesh1->pdir1[match_ind];
				pairs.back().pdir2 = nxf1 * mesh1->pdir2[match_ind];
				pairs.back().curv1 = mesh1->curv1[match_ind];
				pairs.back().curv2 = mesh1->curv2[match_ind];
			}

		}

	} else if (variant <= VARIANT_PT2TPL_ONEWAY) {
		for (size_t i = 0; i < nv1; i++) {
			// Transform into coords of mesh2 and match
			point p12 = xf12 * mesh1->vertices[i];

			const float *match = kd2->closest_to_pt(p12, maxdist2, approx_eps);
			if (!match)
				continue;

			int match_ind = (const point *) match - &(mesh2->vertices[0]);

			// A fast yet not accurate method to calculate the NN point (or NN face)
			// First, we include the several faces ``near'' NN into ``concerned set''
			set<int> concerned_faces_id;
			for (auto &f_id: mesh2->adjacentfaces[match_ind]) {
				concerned_faces_id.insert(f_id);
			}
			for (auto &nb_v_id: mesh2->neighbors[match_ind]) {
				for (auto &f_id: mesh2->adjacentfaces[nb_v_id]) {
					concerned_faces_id.insert(f_id);
				}
			}
			// Next, we search for NN point and face among the concerned faces
			double min_sq_dist = INFTY;
			point nn_pt;
			int nn_f_i;
			point fv[3];
			for (auto &i: concerned_faces_id) {
				for (int j = 0; j < 3; j++) {
					fv[j] = mesh2->vertices[mesh2->faces[i][j]];
				}
				point tmp_res;
				int flag;
				double new_sq_dist = sq_dist_p2f(p12, fv, tmp_res, flag);

				if (min_sq_dist > new_sq_dist) {
					min_sq_dist = new_sq_dist;
					nn_pt = tmp_res;
					nn_f_i = i;
				}
			}
			vec nn_norm = mesh2->trinorm(nn_f_i);

			// Project both points into world coords and save
			point p1 = xf1 * mesh1->vertices[i];
			vec n1 = nxf1 * mesh1->normals[i];
			point p2 = xf2 * nn_pt;
			vec n2 = nxf2 * nn_norm;

			if ((n1 DOT n2) < 0.0f)
				continue;

			if (do_flip)
				pairs.push_back(PtPair(p2, n2, p1, n1));
			else
				pairs.push_back(PtPair(p1, n1, p2, n2));

		}

	} else if (variant <= VARIANT_CT2PL_SYMM_ROTNORM) {
		TriMesh* mesh_ct2 = new TriMesh;
		for (int i = 0; i < mesh2->faces.size(); i++) {
			mesh_ct2->vertices.push_back(mesh2->centroid(i));
		}

		KDtree* kd_ct2 = new KDtree(mesh_ct2->vertices);

		for (int i = 0; i < nf1; i++) {
			// Transform ct into coords of mesh2 and match
			point p12 = xf12 * mesh1->centroid(i);

			const float *match;
			// if (normdot_thresh >= 0.0f) {
			// 	vec n12 = nxf12 * mesh1->normals[i];
			// 	NormCompat nc(n12, mesh2, normdot_thresh);
			// 	match = kd2->closest_to_pt(p12, maxdist2, &nc, approx_eps);
			// } else {
				match = kd_ct2->closest_to_pt(p12, maxdist2, approx_eps);
			// }
			if (!match)
				continue;

			int match_ind = (const point *) match - &(mesh_ct2->vertices[0]);

			// Project both points into world coords and save
			point p1 = xf1 * mesh1->centroid(i);
			vec n1 = nxf1 * mesh1->trinorm(i);
			point p2 = xf2 * mesh_ct2->vertices[match_ind];
			vec n2 = nxf2 * mesh2->trinorm(match_ind);
			if ((n1 DOT n2) < 0.0f)
				continue;

			if (do_flip)
				pairs.push_back(PtPair(p2, n2, p1, n1));
			else
				pairs.push_back(PtPair(p1, n1, p2, n2));

		}

	} else if (variant <= VARIANT_CT2NNPL_SYMM_ROTNORM)  {
		for (int i = 0; i < nf1; i++) {
			// Transform ct into coords of mesh2 and match
			point p12 = xf12 * mesh1->centroid(i);

			const float *match = kd2->closest_to_pt(p12, maxdist2, approx_eps);
			if (!match)
				continue;

			int match_ind = (const point *) match - &(mesh2->vertices[0]);

			// A fast yet not accurate method to calculate the NN point (or NN face)
			// First, we include the several faces ``near'' NN into ``concerned set''
			set<int> concerned_faces_id;
			for (auto &f_id: mesh2->adjacentfaces[match_ind]) {
				concerned_faces_id.insert(f_id);
			}
			for (auto &nb_v_id: mesh2->neighbors[match_ind]) {
				for (auto &f_id: mesh2->adjacentfaces[nb_v_id]) {
					concerned_faces_id.insert(f_id);
				}
			}
			// Next, we search for NN point and face among the concerned faces
			double min_sq_dist = INFTY;
			point nn_pt;
			int nn_f_i;
			point fv[3];
			for (auto &i: concerned_faces_id) {
				for (int j = 0; j < 3; j++) {
					fv[j] = mesh2->vertices[mesh2->faces[i][j]];
				}
				point tmp_res;
				int flag;
				double new_sq_dist = sq_dist_p2f(p12, fv, tmp_res, flag);

				if (min_sq_dist > new_sq_dist) {
					min_sq_dist = new_sq_dist;
					nn_pt = tmp_res;
					nn_f_i = i;
				}
			}
			vec nn_norm = mesh2->trinorm(nn_f_i);

			// Project both points into world coords and save
			point p1 = xf1 * mesh1->centroid(i);
			vec n1 = nxf1 * mesh1->trinorm(i);
			point p2 = xf2 * nn_pt;
			vec n2 = nxf2 * nn_norm;

			if ((n1 DOT n2) < 0.0f)
				continue;

			if (do_flip)
				pairs.push_back(PtPair(p2, n2, p1, n1));
			else
				pairs.push_back(PtPair(p1, n1, p2, n2));

		}

	}

}


// Sample on mesh1, find closest on mesh2, append to pairs.
// Doesn't bother filling in curv and pdir for each pair unless VARIANT_MITRA
static void select_and_match(const TriMesh *mesh1, const TriMesh *mesh2,
                             const xform &xf1, const xform &xf2,
                             const KDtree *kd2, float cdfincr,
                             float approx_eps, float maxdist,
                             float normdot_thresh, ICP_Variant variant,
                             bool do_flip, ::std::vector<PtPair> &pairs)
{
	size_t nv1 = mesh1->vertices.size();
	xform nxf1 = norm_xf(xf1);
	xform nxf2 = norm_xf(xf2);
	xform xf12 = inv(xf2) * xf1;
	xform nxf12 = norm_xf(xf12);
	float maxdist2 = sqr(maxdist);

	size_t i = 0;
	float cdfval = uniform_rnd(cdfincr);
	while (cdfval < 1.0f) {
		i = size_t(nv1 * cdfval);

		// Set up cdfval for the next iteration
		cdfval += cdfincr;

		// Transform into coords of mesh2 and match
		point p12 = xf12 * mesh1->vertices[i];

		const float *match;
		if (normdot_thresh >= 0.0f) {
			vec n12 = nxf12 * mesh1->normals[i];
			NormCompat nc(n12, mesh2, normdot_thresh);
			match = kd2->closest_to_pt(p12, maxdist2, &nc, approx_eps);
		} else {
			match = kd2->closest_to_pt(p12, maxdist2, approx_eps);
		}
		if (!match)
			continue;

		int match_ind = (const point *) match - &(mesh2->vertices[0]);

		// Project both points into world coords and save
		point p1 = xf1 * mesh1->vertices[i];
		vec n1 = nxf1 * mesh1->normals[i];
		point p2 = xf2 * mesh2->vertices[match_ind];
		vec n2 = nxf2 * mesh2->normals[match_ind];
		if ((n1 DOT n2) < 0.0f)
			continue;

		if (do_flip)
			pairs.push_back(PtPair(p2, n2, p1, n1));
		else
			pairs.push_back(PtPair(p1, n1, p2, n2));

		if (variant != VARIANT_MITRA)
			continue;

		if (do_flip) {
			pairs.back().pdir1 = nxf2 * mesh2->pdir1[match_ind];
			pairs.back().pdir2 = nxf2 * mesh2->pdir2[match_ind];
			pairs.back().curv1 = mesh2->curv1[match_ind];
			pairs.back().curv2 = mesh2->curv2[match_ind];
		} else {
			pairs.back().pdir1 = nxf1 * mesh1->pdir1[match_ind];
			pairs.back().pdir2 = nxf1 * mesh1->pdir2[match_ind];
			pairs.back().curv1 = mesh1->curv1[match_ind];
			pairs.back().curv2 = mesh1->curv2[match_ind];
		}
	}
}


// Find the median of a list of numbers - makes a copy of vals (since it's
// passed by value, not by reference) so that we can modify it
static float median(::std::vector<float> vals)
{
	size_t n = vals.size();
	if (!n)
		return 0.0f;

	size_t mid = n / 2;
	::std::nth_element(vals.begin(), vals.begin() + mid, vals.end());
	return vals[mid];
}


// Perform outlier rejection on a list of point pairs.
// This rejects points whose distance is more than rejection_thresh_mult
// times the median point-to-point distance.
// Also updates maxdist for the next iteration.
static void reject(::std::vector<PtPair> &pairs, float rejection_thresh_mult,
	float &maxdist)
{
	size_t npairs = pairs.size();

	// Compute median point-to-point distance
	::std::vector<float> distances2(npairs);
	for (size_t i = 0; i < npairs; i++)
		distances2[i] = dist2(pairs[i].p1, pairs[i].p2);
	float median_dist = sqrt(median(distances2));

	// Update maxdist and compute rejection threshold
	maxdist = rejection_thresh_mult * median_dist;
	float dist2_thresh = sqr(maxdist);

	// Reject
	size_t next = 0;
	for (size_t i = 0; i < npairs; i++) {
		if (distances2[i] <= dist2_thresh)
			pairs[next++] = pairs[i];
	}
	pairs.erase(pairs.begin() + next, pairs.end());
}


// Perform a single iteration of ICP, updating xf2
static bool ICP_iter(TriMesh *mesh1, TriMesh *mesh2,
                     const xform &xf1, xform &xf2,
                     const KDtree *kd1, const KDtree *kd2,
                     float &cdfincr, int desired_pairs,
                     float approx_eps, float &maxdist,
                     float normdot_thresh, float rejection_thresh_mult,
                     ICP_Variant variant, float lambda)
{
	// Find pairs
	::std::vector<PtPair> pairs;
	if (kd2) {
		// select_and_match(mesh1, mesh2, xf1, xf2, kd2, cdfincr,
		// 	approx_eps, maxdist, normdot_thresh, variant,
		// 	false, pairs);
		tri_select_and_match(mesh1, mesh2, xf1, xf2, kd2, cdfincr,
			approx_eps, maxdist, normdot_thresh, variant,
			false, pairs);
	}
	if (kd1) {
		// select_and_match(mesh2, mesh1, xf2, xf1, kd1, cdfincr,
		// 	approx_eps, maxdist, normdot_thresh, variant,
		// 	true, pairs);
		tri_select_and_match(mesh2, mesh1, xf2, xf1, kd1, cdfincr,
			approx_eps, maxdist, normdot_thresh, variant,
			true, pairs);
	}

	// // Reject
	// reject(pairs, rejection_thresh_mult, maxdist);
	size_t npairs = pairs.size();
	cdfincr *= (float) npairs / desired_pairs;

	// Compute centroids, scale
	point centroid1, centroid2;
	for (size_t i = 0; i < npairs; i++) {
		centroid1 += pairs[i].p1;
		centroid2 += pairs[i].p2;
	}
	centroid1 /= npairs;
	centroid2 /= npairs;

	float scale = 0.0f;
	for (size_t i = 0; i < npairs; i++) {
		scale += dist2(pairs[i].p1, centroid1);
		scale += dist2(pairs[i].p2, centroid2);
	}
	scale = sqrt(scale / (2 * npairs));
	scale = 1.0f / scale;

	// Do the minimization
	xform alignxf;
	bool success = true;
	switch (variant) {
		case VARIANT_PT2PT:
		case VARIANT_PT2NN:
		case VARIANT_CT2CT:
		case VARIANT_CT2NN:
			success = align_pt2pt(pairs,
				centroid1, centroid2, alignxf);
			break;
		case VARIANT_PT2PL_ONEWAY:
		case VARIANT_PT2TPL_ONEWAY:
		case VARIANT_CT2PL_ONEWAY:
		case VARIANT_CT2NNPL_ONEWAY:
			success = align_pt2pl_oneway(pairs,
				scale, centroid1, centroid2, alignxf);
			break;
		case VARIANT_SYMM:
		case VARIANT_CT2PL_SYMM:
		case VARIANT_CT2NNPL_SYMM:
			success = align_symm(pairs,
				scale, centroid1, centroid2, alignxf);
			break;
		case VARIANT_PT2PL_TWOPLANE:
			success = align_pt2pl_twoplane(pairs,
				scale, centroid1, centroid2, alignxf);
			break;
		case VARIANT_SYMM_ROTNORM:
		case VARIANT_CT2PL_SYMM_ROTNORM:
		case VARIANT_CT2NNPL_SYMM_ROTNORM:
			success = align_symm_rotnorm(pairs,
				scale, centroid1, centroid2, alignxf);
			break;
		case VARIANT_MITRA:
			success = align_mitra(pairs,
				scale, centroid1, centroid2, alignxf);
			break;
		case VARIANT_PT2PL_ONEWAY_LM:
			success = align_pt2pl_oneway_lm(pairs,
				scale, centroid1, centroid2, alignxf,
				lambda);
			break;
		case VARIANT_SYMM_LM:
			success = align_symm_lm(pairs,
				scale, centroid1, centroid2, alignxf,
				lambda);
	}

	if (success) {
		// Update xf2
		xf2 = alignxf * xf2;
		orthogonalize(xf2);
	}

	return success;
}

} // namespace trimesh
