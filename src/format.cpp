#include <string>

#include "format.h"

using std::string;

string reformat_timeunit(long time_unit) {
  string tmp = "0" + std::to_string(time_unit);
  return tmp.substr(tmp.length() - 2, 2);
}

string Format::ElapsedTime(long seconds) {
  long hours = seconds / 3600;
  long mins = (seconds % 3600) / 60;
  long secs = seconds % 60;
  string hours_str = reformat_timeunit(hours);
  string mins_str = reformat_timeunit(mins);
  string secs_str = reformat_timeunit(secs);
  return hours_str + ":" + mins_str + ":" + secs_str;
}