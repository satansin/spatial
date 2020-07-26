#include <string>

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

double rand_double_in_range(double low, double high) {
	double r = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
	return (low + r * (high - low));
}