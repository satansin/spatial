#include "TriMesh.h"
#include "KDtree.h"
#include <string>
#include <vector>
#include <limits>

const int fix_len = 7;

std::string get_filename(const std::string path) {
	int found = path.find_last_of("/\\");
	return path.substr(found + 1);
}

std::string get_foldername(const std::string path) {
	std::string ret;
	if (path[path.length() - 1] != '/') {
		ret = path + "/";
	} else {
		ret = path;
	}
	return ret;
}

const double PI = 3.14159265359;

double eucl_dist(trimesh::point p, trimesh::point q) {
	return sqrt((p[0] - q[0]) * (p[0] - q[0]) + (p[1] - q[1]) * (p[1] - q[1]) + (p[2] - q[2]) * (p[2] - q[2]));
}

double rand_double_in_range(double low, double high) {
	double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	return (low + r * (high - low));
}

trimesh::point rand_pt(double x1, double x2, double y1, double y2, double z1, double z2) {
	return trimesh::point(rand_double_in_range(x1, x2), rand_double_in_range(y1, y2), rand_double_in_range(z1, z2));
}

double nn_dist(trimesh::point p, std::vector<trimesh::point> vertices) {
	double nn_dist = std::numeric_limits<double>::max();
	for (int i = 0; i < vertices.size(); i++) {
		double tmp_dist = eucl_dist(p, vertices[i]);
		if (tmp_dist - nn_dist < 0) {
			nn_dist = tmp_dist;
		}
	}
	return nn_dist;
}

double nn_dist(trimesh::point p, trimesh::KDtree* kdtree_p) {
	auto nn = kdtree_p->closest_to_pt(&p[0]);
	trimesh::point nn_p(nn[0], nn[1], nn[2]);
	return eucl_dist(nn_p, p);
}

int read_db_mesh_filenames(const std::string path, std::vector<std::string>& db_filenames) {
	auto db_folder = get_foldername(path);
	std::ifstream ifs(db_folder + "meta.txt");

	if (!ifs) {
		std::cerr << "Fail reading meta file " << (db_folder + "meta.txt") << std::endl;
		exit(1);
	} else {
		std::cout << "Reading " << (db_folder + "meta.txt") << std::endl;
	}

	int num;
	ifs >> num;

	int id;
	std::string s_file;

	for (int i = 0; i < num; i++) {
		ifs >> id >> s_file;
		db_filenames.push_back(s_file);
	}

	ifs.close();

	return num;
}

int read_db_mesh_names(const std::string path, std::vector<std::string>& db_mesh_names) {
	auto db_folder = get_foldername(path);
	std::ifstream ifs(db_folder + "meta.txt");

	if (!ifs) {
		std::cerr << "Fail reading meta file " << (db_folder + "meta.txt") << std::endl;
		exit(1);
	} else {
		std::cout << "Reading " << (db_folder + "meta.txt") << std::endl;
	}

	int num;
    ifs >> num;

	int id;
	std::string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
    	auto fn = get_filename(s_file);
    	// cout << fn.substr(0, fn.length() - 4) << endl;
    	db_mesh_names.push_back(fn.substr(0, fn.length() - 4));
    }

    ifs.close();

    return num;
}

int read_db_meshes(const std::string path, std::vector<trimesh::TriMesh*>& db_meshes) {
	auto db_folder = get_foldername(path);
	std::ifstream ifs(db_folder + "meta.txt");

	if (!ifs) {
		std::cerr << "Fail reading meta file " << (db_folder + "meta.txt") << std::endl;
		exit(1);
	} else {
		std::cout << "Reading " << (db_folder + "meta.txt") << std::endl;
	}

	int num;
    ifs >> num;

	int id;
	std::string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
    	db_meshes.push_back(trimesh::TriMesh::read(db_folder + s_file));
    	std::cout << "Reading mesh #" << id << ": " << s_file << std::endl;
    }

    ifs.close();

    return num;
}

int read_db_meshes_n_names(const std::string path, std::vector<trimesh::TriMesh*>& db_meshes, std::vector<std::string>& db_mesh_names) {
	auto db_folder = get_foldername(path);
	std::ifstream ifs(db_folder + "meta.txt");

	if (!ifs) {
		std::cerr << "Fail reading meta file " << (db_folder + "meta.txt") << std::endl;
		exit(1);
	} else {
		std::cout << "Reading " << (db_folder + "meta.txt") << std::endl;
	}

	int num;
    ifs >> num;

	int id;
	std::string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
    	db_meshes.push_back(trimesh::TriMesh::read(s_file));
    	auto fn = get_filename(s_file);
    	// cout << fn.substr(0, fn.length() - 4) << endl;
    	db_mesh_names.push_back(fn.substr(0, fn.length() - 4));
    	std::cout << "Reading mesh #" << id << ": " << s_file << std::endl;
    }

    ifs.close();

    return num;
}

int read_random_db_mesh(const std::string path, trimesh::TriMesh*& db_mesh) {
	auto db_folder = get_foldername(path);
	std::ifstream ifs(db_folder + "meta.txt");

	if (!ifs) {
		std::cerr << "Fail reading meta file " << (db_folder + "meta.txt") << std::endl;
		exit(1);
	} else {
		std::cout << "Reading " << (db_folder + "meta.txt") << std::endl;
	}

	int num;
    ifs >> num;
    const int rand_id = rand() % num;

	int id;
	std::string s_file;

    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        if (i == rand_id) {
        	db_mesh = trimesh::TriMesh::read(s_file);
        	std::cout << "Reading mesh #" << id << ": " << s_file << std::endl;
        	break;
        }
    }

    ifs.close();

    return rand_id;
}

void read_db_mesh_batch(std::vector<std::string>& db_filenames, int batch_i, int batch_size, std::vector<trimesh::TriMesh*>& db_meshes) {
    for (int i = 0; i < batch_size; i++) {
    	int real_i = batch_i * batch_size + i;
    	db_meshes.push_back(trimesh::TriMesh::read(db_filenames[real_i]));
    }
}

void append_mesh(trimesh::TriMesh* src_mesh, trimesh::TriMesh* tar_mesh, int from = -1, int size = -1) {
	if (from < 0 || size < 0) {
		for (auto &p: src_mesh->vertices) {
			tar_mesh->vertices.push_back(p);
		}
	} else {
		for (int i = from; i < from + size; i++) {
			tar_mesh->vertices.push_back(src_mesh->vertices[i]);
		}
	}
}

void read_db_meshes_combined(const std::string path, std::vector<trimesh::TriMesh*>& db_meshes) {
	auto db_folder = get_foldername(path);

    std::ifstream ifs_combined_meta(db_folder + "combined.meta");
    int num_parts;
    ifs_combined_meta >> num_parts;

    std::vector<int> part_sizes;
    int ps;
    for (int i = 0; i < num_parts; i++) {
    	ifs_combined_meta >> ps;
    	part_sizes.push_back(ps);
    }
    
    ifs_combined_meta.close();

    for (int i = 0; i < num_parts; i++) {
    	int part_size = part_sizes[i];

    	std::vector<int> pt_sizes;
    	std::ifstream ifs_combined_part(db_folder + "combined.meta." + std::to_string(i));
    	int pts;
    	for (int j = 0; j < part_size; j++) {
    		ifs_combined_part >> pts;
    		pt_sizes.push_back(pts);
    	}
    	ifs_combined_part.close();

    	trimesh::TriMesh* mesh_part = trimesh::TriMesh::read(db_folder + "combined.ply." + std::to_string(i));
    	int from = 0;
    	for (int j = 0; j < part_size; j++) {
    		int pt_size = pt_sizes[j];

    		trimesh::TriMesh* new_mesh = new trimesh::TriMesh;
    		append_mesh(mesh_part, new_mesh, from, pt_size);
    		from += pt_size;

    		db_meshes.push_back(new_mesh);
    	}

    	mesh_part->clear();
    	delete mesh_part;
    	std::vector<int>().swap(pt_sizes);
    }
}

void read_db_meshes_combined_by_part(const std::string path, int part_i, std::vector<trimesh::TriMesh*>& db_meshes) {
	auto db_folder = get_foldername(path);

    std::ifstream ifs_combined_meta(db_folder + "combined.meta");
    int num_parts;
    ifs_combined_meta >> num_parts;

    if (part_i < 0 || part_i >= num_parts) {
    	std::cerr << "Wrong part_i: " << part_i << std::endl;
    	ifs_combined_meta.close();
    	return;
    }

    std::vector<int> part_sizes;
    int ps;
    for (int i = 0; i < num_parts; i++) {
    	ifs_combined_meta >> ps;
    	part_sizes.push_back(ps);
    }
    
    ifs_combined_meta.close();

    for (int i = 0; i < num_parts; i++) {
    	if (i != part_i) {
    		continue;
    	}

    	int part_size = part_sizes[i];

    	std::vector<int> pt_sizes;
    	std::ifstream ifs_combined_part(db_folder + "combined.meta." + std::to_string(i));
    	int pts;
    	for (int j = 0; j < part_size; j++) {
    		ifs_combined_part >> pts;
    		pt_sizes.push_back(pts);
    	}
    	ifs_combined_part.close();

    	trimesh::TriMesh* mesh_part = trimesh::TriMesh::read(db_folder + "combined.ply." + std::to_string(i));
    	int from = 0;
    	for (int j = 0; j < part_size; j++) {
    		int pt_size = pt_sizes[j];

    		trimesh::TriMesh* new_mesh = new trimesh::TriMesh;
    		append_mesh(mesh_part, new_mesh, from, pt_size);
    		from += pt_size;

    		db_meshes.push_back(new_mesh);
    	}

    	mesh_part->clear();
    	delete mesh_part;
    	std::vector<int>().swap(pt_sizes);
    }
}