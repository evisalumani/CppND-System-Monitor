#include <string>

#include "format.h"

using std::string;

const int SECONDS_IN_HOUR = 60*60;
const int SECONDS_IN_MINUTE = 60;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds/SECONDS_IN_HOUR;
    int remainingSec = seconds % SECONDS_IN_HOUR;
    int minutes = remainingSec/SECONDS_IN_MINUTE;
    int seconds_ = remainingSec % SECONDS_IN_MINUTE;
    
    string formattedTime = formatTimeComponent(hours) + ":" + formatTimeComponent(minutes) + ":" + formatTimeComponent(seconds_);
    return formattedTime;
}

string Format::formatTimeComponent(int n) {
    if (n<10) {
        return "0" + std::to_string(n);
    }
    return std::to_string(n);
}