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

#endif