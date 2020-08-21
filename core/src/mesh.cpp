#include "util.h"
#include "mesh.h"

using namespace std;
using namespace trimesh;

Mesh::Mesh(int id) {
	m_id = id;
	m_size = 0;
	m_bbox_built = false;
}

void Mesh::read_from_path(const string s_file) {
	m_filename = s_file;
	m_mesh = TriMesh::read(s_file);
	m_size = m_mesh->vertices.size();

    m_pt_list.clear();
    m_pt_list.reserve(m_size);
    for (int i = 0; i < m_size; i++) {
        Pt3D t_pt;
        pt(m_mesh->vertices[i], t_pt);
        m_pt_list.push_back(t_pt);
    }
    m_pt_list.shrink_to_fit();
}

int Mesh::size() const {
	return m_size;
}

Pt3D* Mesh::get_pt(int idx) {
	assert(idx >= 0 && idx < m_size);
	return &(m_pt_list[idx]);
}

const string Mesh::get_filename() const {
	return m_filename;
}

void Mesh::build_bbox() {
	if (!m_bbox_built) {
		m_mesh->need_bbox();
		m_bbox_built = true;
	}
}

void Mesh::build_bsphere() {
    if (!m_bsphere_built) {
        m_mesh->need_bsphere();
        m_bsphere_built = true;
    }
}

double Mesh::get_box_min(int i) {
	build_bbox();
	return m_mesh->bbox.min[i];
}

double Mesh::get_box_max(int i) {
	build_bbox();
	return m_mesh->bbox.max[i];
}

double Mesh::get_bsphere_r() {
    build_bsphere();
    return m_mesh->bsphere.r;
}

double Mesh::get_bsphere_d() {
    build_bsphere();
    return 2.0 * m_mesh->bsphere.r;
}

void Mesh::get_bsphere_o(Pt3D& o) {
    build_bsphere();
    o.x = m_mesh->bsphere.center[0];
    o.y = m_mesh->bsphere.center[1];
    o.z = m_mesh->bsphere.center[2];
}

void Mesh::centralize() {
	trans(m_mesh, -mesh_center_of_mass(m_mesh));
}

double Mesh::box_max_scale() {
	build_bbox();
	double ret = -1.0;
	for (int i = 0; i < 3; i++) {
		double box_scale_dim_i = (m_mesh->bbox.max[i] - m_mesh->bbox.min[i]) * 0.5;
		if (box_scale_dim_i > ret) {
			ret = box_scale_dim_i;
		}
	}
	return ret;
}

void Mesh::scale_by(double s) {
	scale(m_mesh, s);
}


string DB_Meshes::get_meta_filename(const string path) const {
	string folder = get_foldername(path);
	return (folder + "meta.txt");
}

DB_Meshes::DB_Meshes() {
	m_total = 0;
	m_size = 0;
	m_kd_built = false;
}

int DB_Meshes::read_from_path(const string db_path) {
    ifstream ifs(get_meta_filename(db_path));

    ifs >> m_size;

    int id;
    string s_file;
    for (int i = 0; i < m_size; i++) {
        ifs >> id >> s_file;

        Mesh new_mesh(id);
        new_mesh.read_from_path(s_file);

        m_total += new_mesh.size();
        m_db_meshes.push_back(new_mesh);
    }

    ifs.close();

    return m_size;
}

void DB_Meshes::build_kd() {
    for (auto &t: m_db_meshes) {
        m_db_kds.push_back(new KDtree(t.m_mesh->vertices));
    }
    m_kd_built = true;
}

bool DB_Meshes::kd_built() const {
	return m_kd_built;
}

int DB_Meshes::size() const {
	return m_size;
}

int DB_Meshes::total() const {
	return m_total;
}

Mesh* DB_Meshes::get_mesh(int id) {
	assert(id >= 0 && id < m_size);
	return &m_db_meshes[id];
}

// const KDtree* get_kd(int id) {
// 	assert(id >= 0 && id < m_size);
// 	return m_db_kds[id];
// }

double DB_Meshes::cal_corr_err(Mesh* mesh_q, int id, Trans* xf, double stop_at) {
	assert(id >= 0 && id < m_size);

    double err = 0.0;
    for (auto &v: mesh_q->m_mesh->vertices) {
        Pt3D q; Mesh::pt(v, q);
    	Pt3D xf_q;
    	if (xf != NULL) {
        	trans_pt(xf, &q, xf_q);
        } else {
        	xf_q = q;
        }
        float pt_arr[3] = { (float) xf_q.x, (float) xf_q.y, (float) xf_q.z };
        auto nn = m_db_kds[id]->closest_to_pt(pt_arr);
        // std::cout << nn << std::endl;
        // TODO: nn != 0!!!
        if (!nn)
            continue;
        double nn_err = sq_dist(nn, pt_arr);
        err += nn_err;
        if (err - stop_at > 0) {
            return -1;
        }
    }
    return err;
}

double DB_Meshes::cal_corr_err(double x, double y, double z, int id) {
	assert(id >= 0 && id < m_size);

	float pt_arr[3] = { (float) x, (float) y, (float) z };
	auto nn = m_db_kds[id]->closest_to_pt(pt_arr);
    if (!nn)
        return 0.0;

    return eucl_dist(nn, pt_arr);
}

void DB_Meshes::retrieve(Mesh* mesh_q, int id, std::unordered_set<int>& ret, Trans* xf) {
	assert(id >= 0 && id < m_size);

	for (auto &v: mesh_q->m_mesh->vertices) {
        Pt3D q; Mesh::pt(v, q);
        Pt3D xf_q;
        if (xf != NULL) {
            trans_pt(xf, &q, xf_q);
        } else {
            xf_q = q;
        }
        float pt_arr[3] = { (float) xf_q.x, (float) xf_q.y, (float) xf_q.z };
        auto nn = m_db_kds[id]->closest_to_pt(pt_arr);
        if (!nn)
            continue;

        int db_pt_id = distance((const float *) &m_db_meshes[id].m_mesh->vertices[0], nn) / 3;

        ret.insert(db_pt_id);
    }
}