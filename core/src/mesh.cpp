#include "util.h"
#include "mesh.h"

using namespace std;
using namespace trimesh;

PtwID Mesh::DEFAULT_PT = { -1, (Pt3D *) nullptr };

Mesh::Mesh(int id) {
	m_id = id;
	m_size = 0;
	m_bbox_built = false;
}

Mesh::~Mesh() {
    free_mesh();
}

Mesh::Mesh(const Mesh* mesh) {
    this->m_id = mesh->m_id;
    this->m_size = mesh->m_size;
    this->m_filename = mesh->m_filename;

    for (auto &v: mesh->m_pt_list) {
        this->m_pt_list.push_back(v);
    }
    for (auto &v: mesh->m_ptwid_list) {
        this->m_ptwid_list.push_back(v);
    }

    this->m_mesh = new trimesh::TriMesh;
    for (auto &v: mesh->m_mesh->vertices) {
        this->m_mesh->vertices.push_back(v);
    }
}

void Mesh::free_mesh() {
    if (m_mesh != nullptr) {
        m_mesh->clear();
        delete m_mesh;
        m_mesh = nullptr;
    }
}

void Mesh::insert_pt(Pt3D pt) {
    this->m_pt_list.push_back(pt);
    this->m_size++;
    trimesh::point p = { static_cast<float>(pt.x), static_cast<float>(pt.y), static_cast<float>(pt.z) };
    this->m_mesh->vertices.push_back(p);
}

void Mesh::write(string filename) {
    m_mesh->write(filename);
}

void Mesh::read_from_path(const string s_file) {
	m_filename = s_file;
	m_mesh = TriMesh::read(s_file);
	m_size = m_mesh->vertices.size();

    m_pt_list.clear();
    m_pt_list.reserve(m_size);
    m_ptwid_list.clear();
    m_ptwid_list.reserve(m_size);
    Pt3D t_pt;
    for (int i = 0; i < m_size; i++) {
        pt(m_mesh->vertices[i], t_pt);
        m_pt_list.push_back(t_pt);
        m_ptwid_list.push_back(PtwID(i, &m_pt_list[i]));
    }
    m_pt_list.shrink_to_fit();
    m_ptwid_list.shrink_to_fit();
}

int Mesh::size() const {
	return m_size;
}

Pt3D* Mesh::get_pt(int idx) {
	assert(idx >= 0 && idx < m_size);
	return &(m_pt_list[idx]);
}

PtwID* Mesh::get_ptwid(int idx) {
    if (idx >= 0 && idx < m_size) {
        return &(m_ptwid_list[idx]);
    } else {
        return &DEFAULT_PT;
    }
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


DB_Meshes::DB_Meshes() {
	m_total = 0;
	m_size = 0;
	m_kd_built = false;
}

DB_Meshes::~DB_Meshes() {
    for (auto &m: m_db_meshes) {
        if (m != nullptr)
            delete m;
    }
    for (auto &v: m_db_kds) {
        if (v != nullptr)
            delete v;
    }
    // for (auto &v: m_icps) {
    //     if (v != nullptr)
    //         delete v;
    // }
    // for (auto &i: m_data_icps) {
    //     if (i != nullptr)
    //         delete(i);
    // }
}

void DB_Meshes::free_mesh() {
    for (auto &v: m_db_meshes) {
        v->free_mesh();
    }
}

void DB_Meshes::read_normal(ifstream& ifs_meta, const string folder) {
    int id;
    string s_file;
    for (int i = 0; i < m_size; i++) {
        ifs_meta >> id >> s_file;

        Mesh* new_mesh = new Mesh(id);
        new_mesh->read_from_path(folder + s_file);

        m_total += new_mesh->size();
        m_db_meshes.push_back(new_mesh);
    }
}

void DB_Meshes::read_combined(ifstream& ifs_meta, ifstream& ifs_combined, string folder) {
    int num_parts;
    ifs_combined >> num_parts;
    cout << "num_parts: " << num_parts << endl;

    int part_size;
    for (int i = 0; i < num_parts; i++) {
        ifs_combined >> part_size;

        vector<int> pt_sizes;
        ifstream ifs_combined_part(folder + "combined.meta." + to_string(i));
        int pts;
        for (int j = 0; j < part_size; j++) {
            ifs_combined_part >> pts;
            pt_sizes.push_back(pts);
        }
        ifs_combined_part.close();

        TriMesh* mesh_part = TriMesh::read(folder + "combined.ply." + std::to_string(i));
        
        int id;
        string s_file;
        int from = 0;
        for (int j = 0; j < part_size; j++) {
            ifs_meta >> id >> s_file;

            // cout << from << " " << pt_sizes[j] << endl;

            Mesh* new_mesh = new Mesh(id);
            new_mesh->m_filename = s_file;
            new_mesh->m_mesh = new TriMesh;
            new_mesh->m_size = pt_sizes[j];
            new_mesh->m_pt_list.clear();
            new_mesh->m_pt_list.reserve(pt_sizes[j]);
            new_mesh->m_ptwid_list.clear();
            new_mesh->m_ptwid_list.reserve(pt_sizes[j]);
            Pt3D t_pt;
            for (int k = 0; k < pt_sizes[j]; k++) {
                new_mesh->m_mesh->vertices.push_back(mesh_part->vertices[from + k]);
                Mesh::pt(mesh_part->vertices[from + k], t_pt);
                new_mesh->m_pt_list.push_back(t_pt);
                new_mesh->m_ptwid_list.push_back(PtwID(k, &(new_mesh->m_pt_list[k])));
            }
            new_mesh->m_pt_list.shrink_to_fit();
            new_mesh->m_ptwid_list.shrink_to_fit();

            m_db_meshes.push_back(new_mesh);

            from += pt_sizes[j];
        }

        m_total += mesh_part->vertices.size();

        mesh_part->clear();
        delete mesh_part;
        vector<int>().swap(pt_sizes);
    }

}

int DB_Meshes::read_from_path(const string db_path) {
    const string folder = get_foldername(db_path);
    const string meta_filename = folder + "meta.txt";

    ifstream ifs(meta_filename);
    if (!ifs.is_open()) {
        return -1;
    }
    ifs >> m_size;

    ifstream ifs_combined(folder + "combined.meta");

    if (ifs_combined.is_open()) {
        cout << "Start combined reading" << endl;
        read_combined(ifs, ifs_combined, folder);
    } else {
        cout << "Start normal reading" << endl;
        read_normal(ifs, folder);
    }

    ifs_combined.close();
    ifs.close();

    return m_size;
}

int DB_Meshes::read_from_file(const string db_file) {
    Mesh* new_mesh = new Mesh(0);
    new_mesh->read_from_path(db_file);

    m_total += new_mesh->size();
    m_db_meshes.push_back(new_mesh);
    m_size = 1;

    return m_size;
}

void DB_Meshes::build_kd() {
    for (auto &t: m_db_meshes) {
        m_db_kds.push_back(new KDtree(t->m_mesh->vertices));
    }
    m_kd_built = true;
}

bool DB_Meshes::kd_built() const {
	return m_kd_built;
}

// void DB_Meshes::build_icp(double err_diff) {
//     for (auto &t: m_db_meshes) {
//         auto new_icp = new ICP3D<float>();
//         new_icp->err_diff_def = err_diff;
//         new_icp->do_trim = false;

//         int t_size = t->size();
//         auto new_data_icp = (float *) calloc(3 * t_size, sizeof(float));
//         int i, j;
//         for(i = 0, j = 0; i < t_size; i++) {
//             new_data_icp[j++] = t->get_pt(i)->x;
//             new_data_icp[j++] = t->get_pt(i)->y;
//             new_data_icp[j++] = t->get_pt(i)->z;
//         }
//         // Build ICP kdtree with model dataset
//         new_icp->Build(new_data_icp, t_size);

//         m_icps.push_back(new_icp);
//         m_data_icps.push_back(new_data_icp);
//     }

//     // icp3d.err_diff_def = MSEThresh/100;
//     // if (verbose) printf("icp3d.err_diff_def:%.6f\n", icp3d.err_diff_def);

//     m_icp_init = true;
// }

// bool DB_Meshes::icp_built() const {
//     return m_kd_built;
// }

int DB_Meshes::size() const {
	return m_size;
}

int DB_Meshes::total() const {
	return m_total;
}

Mesh* DB_Meshes::get_mesh(int id) {
	assert(id >= 0 && id < m_size);
	return m_db_meshes[id];
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

        int db_pt_id = distance((const float *) &m_db_meshes[id]->m_mesh->vertices[0], nn) / 3;

        ret.insert(db_pt_id);
    }
}