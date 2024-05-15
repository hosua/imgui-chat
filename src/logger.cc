#include "logger.hh"

std::ostream &operator<<(std::ostream &out, const TimeStamp &ts) {
	std::time_t t = std::time(nullptr);
    out << "[" << std::put_time(std::localtime(&t), "%F %T %Z") << "] ";
    return out;
}
