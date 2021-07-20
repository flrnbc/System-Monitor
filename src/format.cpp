#include <string>
#include <iomanip> // for std::setw, std::setfill
#include <sstream>
#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

// helper function
string PadZeros(long n, int k) {
    std::ostringstream ss;

    ss << std::setw(k) << std::setfill('0') << n; // note '0'! (C char?)
    return ss.str();
}

string Format::ElapsedTime(long seconds) {
    long h, m, s;

    h = seconds/3600;
    m = (seconds - h*3600)/60;
    s = (seconds - h*3600 - m*60);

    return PadZeros(h, 2) + ":" + PadZeros(m, 2) + ":" + PadZeros(s, 2);
}
