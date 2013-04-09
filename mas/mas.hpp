#ifndef MAS_HPP
#define MAS_HPP

#include <vector>
#include <string>
#include <sstream>

inline std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream strs{str};
    std::string tmp;
    while (std::getline(strs, tmp, delim)) result.push_back(tmp);
    return result;
}

#endif
