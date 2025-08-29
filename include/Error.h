#ifndef ERROR_H
#define ERROR_H

#include <string>

namespace Err {
	void setCurrentLine(int line);
	int getCurrentLine();
	void print(const std::string& tag, const std::string& msg);
	inline void parseError(const std::string& msg) { print("parse error", msg); }
	inline void error(const std::string& msg) { print("error", msg); }
	inline void fatal(const std::string& msg) { print("fatal", msg); }
}

#endif


