#include "Error.h"
#include <iostream>

static int g_currentLine = 0;

void Err::setCurrentLine(int line) { g_currentLine = line; }
int Err::getCurrentLine() { return g_currentLine; }

void Err::print(const std::string& tag, const std::string& msg) {
	if (g_currentLine > 0)
		std::cout << "[" << tag << "] line " << g_currentLine << ": " << msg << std::endl;
	else
		std::cout << "[" << tag << "] " << msg << std::endl;
}


