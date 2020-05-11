#ifndef __UTIL_S_H_
#define __UTIL_S_H_

#include <string>

extern const char* TAB;
extern const char* TABTAB;
extern const char* TABTABTAB;

std::string get_foldername(std::string path);

enum Time_Unit {
    MILLISECOND, SECOND, MINUTE, HOUR
};

// extern std::stack<std::chrono::time_point<std::chrono::high_resolution_clock>> timer_stack;

void timer_start();

double timer_end(Time_Unit unit);

struct Exec_stat {
    double user_time;
    double prop_time;
    double veri_time;
    double retr_time;
    int veri_size;
    int num_icp_only;
    int num_goicp;
    int num_verified;
    double first_verified_time;
    int first_veri_size;
    double first_corr_dist;
    double first_olp_rate;
    double best_corr_dist; // with largest olp rate
    double best_olp_rate; // with largest olp rate
    double opt_corr_dist; // with smallest corr dist
    double opt_olp_rate; // with smallest corr dist
};

#endif