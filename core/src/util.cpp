#include "util.h"

#include <stack>

#ifndef WIN32
#include <sys/resource.h>
#include <sys/times.h>
#endif

using namespace std;


#ifndef WIN32
float  userTime_read, sysTime_read;
float  userTime_sort, sysTime_sort;
float  userTime_write, sysTime_write;
float  userTime_overall, sysTime_overall;
struct rusage myTime_program_start, myTime_read_end, myTime_sort_end, myTime_write_end;
#endif


#ifndef WIN32
float calculateExecutionTime(struct rusage *myTimeStart, struct rusage *myTimeEnd/*, float *userTime, float *sysTime*/)
{
    return (
        ((float) (myTimeEnd->ru_utime.tv_sec  - myTimeStart->ru_utime.tv_sec)) +
        ((float) (myTimeEnd->ru_utime.tv_usec - myTimeStart->ru_utime.tv_usec)) * 1e-6
    );
    // (*sysTime) =
    //     ((float) (myTimeEnd->ru_stime.tv_sec  - myTimeStart->ru_stime.tv_sec)) +
    //     ((float) (myTimeEnd->ru_stime.tv_usec - myTimeStart->ru_stime.tv_usec)) * 1e-6;
    
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
    return interval;
}