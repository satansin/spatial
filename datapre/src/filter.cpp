#include <dirent.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "TriMesh.h"
using namespace std;
using namespace trimesh;

string get_foldername(string path) {
	string ret;
	if (path[path.length() - 1] != '/') {
		ret = path + "/";
	} else {
		ret = path;
	}
	return ret;
}

bool process_single(string input_filename, string output_filename, double scale_val) {

	TriMesh* input_mesh = TriMesh::read(input_filename);
	if (!input_mesh) {
		return false;
	}

	TriMesh* output_mesh = new TriMesh;

	for (int i = 0; i < input_mesh->vertices.size(); i++) {
		auto v = input_mesh->vertices[i];
		v[0] *= scale_val;
		v[1] *= scale_val;
		v[2] *= scale_val;
		output_mesh->vertices.push_back(v);
	}

	output_mesh->write(output_filename);

	return true;

}

int main(int argc, char** argv) {
	srand(time(0));

	string input_filename = argv[1];
	string output_filename = argv[2];
	double scale_val = atof(argv[3]);

	bool batch = false;
	if (argc > 4 && string(argv[4]) == "-batch") {
		batch = true;
	}

	if (batch) {
		DIR *dir;
		struct dirent *ent;
		string input_foldername = get_foldername(input_filename);
		dir = opendir(input_foldername.c_str());
		if (dir == NULL) {
			cerr << "Could not open folder " << input_foldername << endl;
			exit(1);
		}

		vector<string> s_files;
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_name[0] == '.') {
				continue; // skip . and ..
			}
			string s_file(ent->d_name);
			// cout << s_file << endl;
			s_files.push_back(s_file);
		}

		if (s_files.empty()) {
			cerr << "No file under folder" << input_foldername << endl;
			exit(1);
		}

		string output_foldername = get_foldername(output_filename);

		ofstream ofs_meta(output_foldername + "meta.txt");
		ofs_meta << s_files.size() << endl;

		for (int i = 0; i < s_files.size(); i++) {
			string input_realname = input_foldername + s_files[i];
			string output_realname = output_foldername + s_files[i];
			if (process_single(input_realname, output_realname, scale_val)) {
				ofs_meta << i << " " << output_realname << endl;
				cout << "Input " << input_realname << " processed and saved into " << output_realname << endl;
			}
		}

		ofs_meta.close();
		closedir(dir);

	} else {
		string output_foldername = get_foldername(output_filename.substr(0, output_filename.rfind('/')));

		ofstream ofs_meta(output_foldername + "meta.txt");
		ofs_meta << 1 << endl;

		if (process_single(input_filename, output_filename, scale_val)) {
			ofs_meta << 0 << " " << output_filename << endl;
			cout << "Input " << input_filename << " processed and saved into " << output_filename << endl;
		}
	}
}