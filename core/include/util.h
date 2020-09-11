#ifndef __UTIL_S_H_
#define __UTIL_S_H_

#include <string>
#include <vector>

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
    double retr_time; // not used yet
    double veri_size; // double type due to avg

    int num_icp_only;
    int num_goicp;
    int num_verified;

    double first_verified_time;
    double first_veri_size;
    double first_corr_dist;
    double first_olp_rate;
    double best_corr_dist; // with largest olp rate
    double best_olp_rate; // with largest olp rate
    double opt_corr_dist; // with smallest corr dist
    double opt_olp_rate; // with smallest corr dist

    double num_iterations; // double type due to avg

    double cal_entries_time;
    double retrieve_congr_time;
    double num_entries; // double type due to avg
    double num_init_pairs; // double type due to avg
    double num_db_tetra_matchings; // double type due to avg

    double prop_excpq_time;
    double cpq_total_time;
    double cpq_prop_time;
    double cpq_ovh_time;

    int num_fail;
    int num_exec;
};

void print_stat(const Exec_stat& stat);
void write_stat(const Exec_stat& stat, std::string filename);
void get_sum_stat(Exec_stat stats[], int num, Exec_stat& ret);
void get_avg_stat(Exec_stat stats[], int exec_times, Exec_stat& ret);

#endif