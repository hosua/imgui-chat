#pragma once

#include <fstream>
#include <ostream>
#include <iomanip>
#include <iostream>

#define LOG_FILE "log.txt"

class TimeStamp {};

std::ostream &operator<<(std::ostream &out, const TimeStamp &ts);

static TimeStamp ts;

class Logger {
public:
	static void log(const std::string& msg, const std::string& path=LOG_FILE){
		std::fstream file(path, std::ios::out | std::ios::in | std::ios::app);
		if (file.is_open())
			file << getTimeStamp() << msg << '\n';
		else
			std::cout << "Could not open file: " << path << "\n";
		file.close();
	}

	static const std::string getTimeStamp(){
		std::ostringstream oss;
		oss << ts;
		return oss.str();
	}
};
