#ifndef __PAIR_DIST_H_
#define __PAIR_DIST_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

struct Pair_Dist {
	int m_db_id;
	int m_ep1;
	int m_ep2;
	double m_dist;
	Pair_Dist() { }
	Pair_Dist(int db_id, int ep1, int ep2, double dist): m_db_id {db_id}, m_ep1 {ep1}, m_ep2 {ep2}, m_dist {dist} { }
};

struct Pair_Dist_Bin {
	int m_bin_id;
	vector<Pair_Dist> m_pd_list;
	Pair_Dist_Bin() {}
	Pair_Dist_Bin(int bin_id): m_bin_id {bin_id} {}
	void insert(int db_id, int ep1, int ep2, double dist) {
		m_pd_list.emplace_back(db_id, ep1, ep2, dist);
	}
	struct Bin_Struct {
		int bin_id;
		int pd_list_size;
	};
};

struct Pair_Dist_Hash {
	// static const unordered_set<double> avail_bin_size = { 0.001, 0.002, 0.004, 0.005, 0.008, 0.01, 0.02, 0.025, 0.04, 0.05, 0.1, 0.2, 0.25, 0.5, 1.0 };
	// static is_bin_size_avail(double bin_size) {
	// 	if (avail_bin_size.find(bin_size) == avail_bin_size.end()) {
	// 		return false;
	// 	} else {
	// 		return true;
	// 	}
	// }
	double m_bin_size;
	Pair_Dist_Hash(double bin_size) {
		// if (!Pair_Dist_Hash::is_bin_size_avail(bin_size) || bin_size > 1.0) {
		// 	cerr << "bin size " << bin_size << " not allowed" << endl;
		// 	exit(0);
		// }
		this->m_bin_size = bin_size;
	}
	Pair_Dist_Hash(const string filename) {
		read_bin(filename);
	}
	Pair_Dist_Hash() { }

	int get_bin_id(double dist) {
		return static_cast<int>(dist / m_bin_size);
	}

	unordered_map<int, Pair_Dist_Bin> m_bin_map;
	void insert(int db_id, int ep1, int ep2, double dist) {
		int bin_id = get_bin_id(dist);
		if (m_bin_map.find(bin_id) == m_bin_map.end()) {
			m_bin_map[bin_id] = Pair_Dist_Bin(bin_id);
		}
		m_bin_map[bin_id].insert(db_id, ep1, ep2, dist);
	}

	void write(const string filename) {
		ofstream ofs(filename);
		if (!ofs.is_open()) {
			cerr << "Fail open " << filename << " for write" << endl;
			exit(1);
		}

		ofs << m_bin_size << " " << m_bin_map.size() << endl;
		for (auto &v: m_bin_map) {
			auto bin = v.second;
			ofs << bin.m_bin_id << " " << bin.m_pd_list.size() << endl;
			for (auto &p: bin.m_pd_list) {
				ofs << p.m_db_id << " " << p.m_ep1 << " " << p.m_ep2 << " " << p.m_dist << endl;
			}
		}

		ofs.close();
	}

	struct Bin_Struct {
		double bin_size;
		int map_size;
	};
	void write_bin(const string filename) {
		const string bin_filename = filename + ".bin";
		ofstream ofs(bin_filename, ios::out | ios::binary);
		if (!ofs.is_open()) {
			cerr << "Fail open " << bin_filename << " for write" << endl;
			exit(1);
		}

		// cout << sizeof(Bin_Struct) << endl;
		// cout << sizeof(Pair_Dist_Bin::Bin_Struct) << endl;
		// cout << sizeof(Pair_Dist) << endl;
		Bin_Struct bs { m_bin_size, (int) m_bin_map.size() };
		ofs.write((char *) &bs, sizeof(Bin_Struct));
		for (auto &v: m_bin_map) {
			auto bin = v.second;
			Pair_Dist_Bin::Bin_Struct pdb_bs { bin.m_bin_id, (int) bin.m_pd_list.size() };
			ofs.write((char *) &pdb_bs, sizeof(Pair_Dist_Bin::Bin_Struct));
			for (auto &p: bin.m_pd_list) {
				ofs.write((char *) &p, sizeof(Pair_Dist));
			}
		}

		ofs.close();
	}

	// void read(const string filename) {
	// 	ifstream ifs(filename);
	// 	if (!ifs.is_open()) {
	// 		cerr << "Fail open " << filename << " for read" << endl;
	// 		exit(1);
	// 	}

	// 	int bin_map_size;
	// 	ifs >> m_bin_size >> bin_map_size;

	// 	for (int i = 0; i < bin_map_size; i++) {
	// 		int bin_id, pd_list_size;
	// 		ifs >> bin_id >> pd_list_size;
	// 		Pair_Dist_Bin pdb(bin_id);
	// 		for (int j = 0; j < pd_list_size; j++) {
	// 			int db_id, ep1, ep2;
	// 			ifs >> db_id >> ep1 >> ep2;
	// 			pdb.insert(db_id, ep1, ep2);
	// 		}
	// 		m_bin_map[bin_id] = pdb;
	// 	}
	// }

	void read_bin(const string filename) {
		const string bin_filename = filename + ".bin";
		ifstream ifs(bin_filename, ios::in | ios::binary);
		if (!ifs.is_open()) {
			cerr << "Fail open " << bin_filename << " for read" << endl;
			exit(1);
		}

		Pair_Dist_Hash::Bin_Struct bs;
		ifs.read((char *) &bs, sizeof(Pair_Dist_Hash::Bin_Struct));
		m_bin_size = bs.bin_size;

		for (int i = 0; i < bs.map_size; i++) {
			Pair_Dist_Bin::Bin_Struct pdb_bs;
			ifs.read((char *) &pdb_bs, sizeof(Pair_Dist_Bin::Bin_Struct));

			Pair_Dist_Bin pdb(pdb_bs.bin_id);
			for (int j = 0; j < pdb_bs.pd_list_size; j++) {
				Pair_Dist pd;
				ifs.read((char *) &pd, sizeof(Pair_Dist));
				pdb.m_pd_list.push_back(pd);
			}

			m_bin_map[pdb_bs.bin_id] = pdb;
		}
	}

	void query(const double dist_from, const double dist_to, vector<Pair_Dist*>& ret) {
		auto id_from = get_bin_id(dist_from);
		auto id_to = get_bin_id(dist_to);

		for (int id_i = id_from; id_i <= id_to; id_i++) {
			auto ptr_bin = &m_bin_map[id_i];
			if (id_i != id_from && id_i != id_to) {
				for (int i = 0; i < ptr_bin->m_pd_list.size(); i++) {
					ret.push_back(&ptr_bin->m_pd_list[i]);
				}
			} else {
				for (int i = 0; i < ptr_bin->m_pd_list.size(); i++) {
					if (dist_from <= ptr_bin->m_pd_list[i].m_dist && ptr_bin->m_pd_list[i].m_dist <= dist_to) {
						ret.push_back(&ptr_bin->m_pd_list[i]);
					}
				}
			}
		}
	}

};

#endif