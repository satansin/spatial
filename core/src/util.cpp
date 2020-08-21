#include "util.h"

#include <stack>
#include <iostream>
#include <fstream>

#ifndef WIN32
#include <sys/resource.h>
#include <sys/times.h>
#endif

using namespace std;


#ifndef WIN32
float calculateExecutionTime(struct rusage *myTimeStart, struct rusage *myTimeEnd)
{
    return (
        ((float) (myTimeEnd->ru_utime.tv_sec  - myTimeStart->ru_utime.tv_sec)) +
        ((float) (myTimeEnd->ru_utime.tv_usec - myTimeStart->ru_utime.tv_usec)) * 1e-6
    );
}
#endif

const char* TAB = "  ";
const char* TABTAB = "    ";
const char* TABTABTAB = "      ";

string get_foldername(string path) {
    string ret;
    if (path[path.length() - 1] != '/') {
        ret = path + "/";
    } else {
        ret = path;
    }
    return ret;
}

// stack<chrono::time_point<chrono::high_resolution_clock>> timer_stack;

#ifndef WIN32
stack<rusage> timer_stack;
#endif


void timer_start() {
    // timer_stack.push(chrono::high_resolution_clock::now());
    rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    timer_stack.push(ru);
}

double timer_end(Time_Unit unit) {
    if (timer_stack.empty()) {
        return 0.0;
    }
    // auto start = timer_stack.top();
    rusage start = timer_stack.top();
    timer_stack.pop();
    // auto end = chrono::high_resolution_clock::now();
    rusage end;
    getrusage(RUSAGE_SELF, &end);
    // double interval = 0.0;
    double interval = calculateExecutionTime(&start, &end);

    // switch (unit) {
    //     case MILLISECOND: {
    //         chrono::duration<double, milli> span_milli = end - start;
    //         interval = span_milli.count();
    //     }
    //     break;
    //     case SECOND: {
    //         chrono::duration<double> span_sec = end - start;
    //         interval = span_sec.count();
    //     }
    //     break;
    //     case MINUTE: {
    //         chrono::duration<double, ratio<60>> span_min = end - start;
    //         interval = span_min.count();
    //     }
    //     break;
    //     case HOUR: {
    //         chrono::duration<double, ratio<3600>> span_hour = end - start;
    //         interval = span_hour.count();
    //     }
    //     break;
    // }

    switch (unit) {
        case MILLISECOND: {
            interval *= 1000.0;
        }
        break;
        case SECOND:
        break;
        case MINUTE: {
            interval /= 60.0;
        }
        break;
        case HOUR: {
            interval /= 3600.0;
        }
        break;
    }

    return interval;
}

void print_stat(const Exec_stat& stat) {

    cout << "Total user time: " << stat.user_time << "(s)" << endl;

    cout << "  Proposal time: " << stat.prop_time << "(s)" << endl;

    cout << "    Proposal time (exclude CPQ): " << stat.prop_excpq_time << "(s)" << endl;

    cout << "      Calculate entries time (exclude CPQ): " << stat.cal_entries_time << "(s)" << endl;
    cout << "      Retrieve congr time (exclude CPQ): " << stat.retrieve_congr_time << "(s)" << endl;

    cout << "    CPQ time: " << stat.cpq_total_time << "(s)" << endl;

    cout << "      CPQ proposal time: " << stat.cpq_prop_time << "(s)" << endl;
    cout << "      CPQ overhead time: " << stat.cpq_ovh_time << "(s)" << endl;

    cout << "  Verification time: " << stat.veri_time << "(s)" << endl;

    cout << "First verified time: " << stat.first_verified_time << "(s)" << endl;

    cout << endl;

    cout << "Total number of transformations: " << stat.veri_size << endl;

    cout << "  Total number of entries (exclude CPQ): " << stat.num_entries << endl;
    cout << "  Total number of inital pairs (exclude CPQ): " << stat.num_init_pairs << endl;
    cout << "  Avg number of DB tetra matchings (exclude CPQ): " << stat.num_db_tetra_matchings << endl;
    // cout << "Final number of valid transformations: " << stat.num_verified << endl;
    cout << "Number of iterations: " << stat.num_iterations << endl;

    cout << endl;

    cout << "Number of fail: " << stat.num_fail << endl;
    cout << "Number of exec: " << stat.num_exec << endl;

}

void write_stat(const Exec_stat& stat, string filename) {

    // ofstream stat_ofs;
    // stat_ofs.open(qc.stat_filename, ofstream::out | ofstream::app);

    ofstream stat_ofs(filename);

    stat_ofs << "user_time=" << stat.user_time << endl;
    stat_ofs << "prop_time=" << stat.prop_time << endl;
    stat_ofs << "prop_excpq_time=" << stat.prop_excpq_time << endl;
    stat_ofs << "cal_entries_time=" << stat.cal_entries_time << endl;
    stat_ofs << "retrieve_congr_time=" << stat.retrieve_congr_time << endl;
    stat_ofs << "cpq_total_time=" << stat.cpq_total_time << endl;
    stat_ofs << "cpq_prop_time=" << stat.cpq_prop_time << endl;
    stat_ofs << "cpq_ovh_time=" << stat.cpq_ovh_time << endl;
    stat_ofs << "veri_time=" << stat.veri_time << endl;
    stat_ofs << "first_verified_time=" << stat.first_verified_time << endl;

    stat_ofs << "veri_size=" << stat.veri_size << endl;
    stat_ofs << "num_entries=" << stat.num_entries << endl;
    stat_ofs << "num_init_pairs=" << stat.num_init_pairs << endl;
    stat_ofs << "num_db_tetra_matchings=" << stat.num_db_tetra_matchings << endl;
    // stat_ofs << "num_verified=" << stat.num_verified << endl;
    stat_ofs << "num_iterations=" << stat.num_iterations << endl;

    stat_ofs << "num_fail=" << stat.num_fail << endl;
    stat_ofs << "num_exec=" << stat.num_exec << endl;

    stat_ofs.close();

}

void get_sum_stat(Exec_stat stats[], int num, Exec_stat& ret) {

    ret = (const struct Exec_stat) { 0 };

    for (int i = 0; i < num; i++) {
        ret.user_time += stats[i].user_time;
        ret.prop_time += stats[i].prop_time;
        ret.veri_time += stats[i].veri_time;
        ret.veri_size += stats[i].veri_size;

        ret.num_verified += stats[i].num_verified;

        ret.first_verified_time += stats[i].first_verified_time;

        ret.num_iterations += stats[i].num_iterations;

        ret.cal_entries_time += stats[i].cal_entries_time;
        ret.retrieve_congr_time += stats[i].retrieve_congr_time;
        ret.num_entries += stats[i].num_entries;
        ret.num_init_pairs += stats[i].num_init_pairs;

        ret.prop_excpq_time += stats[i].prop_excpq_time;
        ret.cpq_total_time += stats[i].cpq_total_time;
        ret.cpq_prop_time += stats[i].cpq_prop_time;
        ret.cpq_ovh_time += stats[i].cpq_ovh_time;

        if (stats[i].num_verified < 1)
            ret.num_fail++;
    }

    ret.num_exec = num;

}

void get_avg_stat(Exec_stat stats[], int exec_times, Exec_stat& ret) {

    get_sum_stat(stats, exec_times, ret);

    double d_avg_over = static_cast<double>(exec_times);

    ret.user_time /= d_avg_over;
    ret.prop_time /= d_avg_over;
    ret.veri_time /= d_avg_over;
    ret.veri_size /= d_avg_over;

    ret.first_verified_time /= d_avg_over;

    ret.num_iterations /= d_avg_over;

    ret.cal_entries_time /= d_avg_over;
    ret.retrieve_congr_time /= d_avg_over;
    ret.num_entries /= d_avg_over;
    ret.num_init_pairs /= d_avg_over;

    ret.prop_excpq_time /= d_avg_over;
    ret.cpq_total_time /= d_avg_over;
    ret.cpq_prop_time /= d_avg_over;
    ret.cpq_ovh_time /= d_avg_over;

    if (ret.num_entries > 0) {
        ret.num_db_tetra_matchings = ret.num_init_pairs / ret.num_entries;
    }

}