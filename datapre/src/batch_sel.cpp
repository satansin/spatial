#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unistd.h>
using namespace std;

string get_foldername(string path) {
    string ret;
    if (path[path.length() - 1] != '/') {
        ret = path + "/";
    } else {
        ret = path;
    }
    return ret;
}

int read_db_filename_batch(string db_path, vector<string>& db_filenames) {
    string db_folder = get_foldername(db_path);
    string meta_filename = db_folder + "meta.txt";

    ifstream ifs(db_folder + "meta.txt");
    if (!ifs) {
    	cerr << "Fail reading meta file " << meta_filename << endl;
    	return 0;
    }

    int num;
    ifs >> num;

    int id;
    string s_file;
    for (int i = 0; i < num; i++) {
        ifs >> id >> s_file;
        db_filenames.push_back(s_file);
    }

    ifs.close();

    return num;
}

int main(int argc, char* argv[]) {

	if (argc < 4) {
		cerr << "Usage: " << argv[0] << " database_full_path database_target_path number_selected" << endl;
		exit(1);
	}

	string db_full_path(argv[1]);
	string db_tar_path(argv[2]);
	int num_sel = atoi(argv[3]);

	vector<string> db_filenames;
	read_db_filename_batch(db_full_path, db_filenames);

	vector<string> s_files_selected;
	while (num_sel > 0 && !db_filenames.empty()) {
		int rand_id = rand() % db_filenames.size();
		s_files_selected.push_back(db_filenames[rand_id]);
		db_filenames.erase(db_filenames.begin() + rand_id);
		num_sel--;
	}
	int num_output = s_files_selected.size();

	string db_tar_folder = get_foldername(db_tar_path);
	ofstream ofs_meta(db_tar_folder + "meta.txt");
	ofs_meta << num_output << endl;

	int sl_created;
	for (int i = 0; i < num_output; i++){
		string s_file = s_files_selected[i];
		string last_name = s_file.substr(s_file.rfind('/') + 1);
		string t_file = db_tar_folder + last_name;

		sl_created = symlink(s_file.c_str(), t_file.c_str());
		if (sl_created == 0) {
			cout << "Symbolic link " << t_file << " created" << endl;
		} else {
			cout << "Error creating symbolic link " << t_file << endl;
			continue;
		}

		string s_file_rst = s_file + ".rst.0";
		string t_file_rst = t_file + ".rst.0";

		sl_created = symlink(s_file_rst.c_str(), t_file_rst.c_str());
		if (sl_created == 0) {
			cout << "Symbolic link " << t_file_rst << " created" << endl;
		} else {
			cout << "Error creating symbolic link " << t_file_rst << endl;
			continue;
		}

		ofs_meta << i << " " << t_file << endl;
	}

	ofs_meta.close();

}