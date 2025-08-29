#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
	std::vector<Token> tokenize(const std::string& line);
private:
	bool inBlockComment = false;
};

#endif


