#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;

  float total_memory;
  float free_memory;
  float buffers;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total_memory = stof(value);
        }
        else if (key == "MemFree:") {
          free_memory = stof(value);
        }
        else if (key == "Buffers:") {
          buffers = stof(value);
        }
      }
    }
  }
  return (total_memory - buffers - free_memory) / total_memory;
}


long LinuxParser::UpTime() {
  long idle_time;
  long up_time;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
    return up_time;
  }
  return up_time;
}


long LinuxParser::Jiffies() {
  long idle = LinuxParser::IdleJiffies();
  long active = LinuxParser::ActiveJiffies();
  return idle + active;
}


long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  int i = 1;
  long jiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (i>= 13 && i <= 16) {
          jiffies += stol(value);
        }
        i +=1;
      }
    }
  }
  return jiffies;
}


long LinuxParser::ActiveJiffies() {
  vector<string> utilizations = CpuUtilization();
  long active_jiffies = 0;
  for (int i = CPUStates::kUser_; i <= CPUStates::kGuestNice_; i++) {
    if (i != CPUStates::kIdle_ and i != CPUStates::kIOwait_) {
      active_jiffies += stol(utilizations[i]);
    }
  }
  return active_jiffies;
}


long LinuxParser::IdleJiffies() {
  vector<string> utilizations = CpuUtilization();
  return (stol(utilizations[CPUStates::kIdle_]) + stol(utilizations[CPUStates::kIOwait_]));
}


vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key;
  string value;

  vector<string> cpu_util;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          while (linestream >> value) {
            cpu_util.emplace_back(value);
          }
        }
      }
    }
  }
  return cpu_util;
}


int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return 0; 
}


int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return 0;
}


string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      return line;
    }
  }
  return string();
}


string LinuxParser::Ram(int pid) {
  string statusFilePath = kProcDirectory + to_string(pid) + kStatusFilename;
  std::ifstream stream(statusFilePath);
  if (!stream.is_open()) {
    std::cerr << "Unable to open " << statusFilePath << "\n";
    return "";
  }
  string line;
  while (std::getline(stream, line)) {
    if (line.find("VmRSS:") != string::npos) {
      std::istringstream iss(line);
      string label;
      long memoryUsage;
      iss >> label >> memoryUsage;
      return to_string(memoryUsage);
    }
  }
  return "";
}


string LinuxParser::Uid(int pid) { 
  string statusFilePath = kProcDirectory + to_string(pid) + kStatusFilename;
  std::ifstream stream(statusFilePath);
  if (!stream.is_open()) {
    std::cerr << "Unable to open " << statusFilePath << "\n";
    return "";
  }
  string line;
  while (std::getline(stream, line)) {
    if (line.find("Uid:") != string::npos) {
      std::istringstream iss(line);
      string label;
      int realUid;
      iss >> label >> realUid;
      return to_string(realUid);
    }
  }
  return "";
}


string LinuxParser::User(int pid) {
  string userId = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (!stream.is_open()) {
    std::cerr << "Unable to open " << kPasswordPath << "\n";
    return "";
  }
  string line;
  while(getline(stream, line)) {
    std::istringstream iss(line);
    string username, _, uid;
    getline(iss, username, ':');
    getline(iss, _, ':'); // skip password
    getline(iss, uid, ':');
    if (uid == userId) {
      return username;
    }
  }
  return "";
}


long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  long int uptime = LinuxParser::UpTime();
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for ( int i = 1; i < 23; i++){
        linestream >> value;
        if (i == 22) {
          return uptime - (stol(value)/sysconf(_SC_CLK_TCK));
        }
    }
  }
  return uptime;
}
