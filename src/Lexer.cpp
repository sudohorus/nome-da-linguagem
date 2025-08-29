#include "Lexer.h"
#include <cctype>

static bool isIdentStart(char c) {
	return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

static bool isIdentChar(char c) {
	return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::vector<Token> Lexer::tokenize(const std::string& line) {
	std::vector<Token> tokens;
	size_t i = 0;
	while (i < line.size()) {
		char c = line[i];
		if (std::isspace(static_cast<unsigned char>(c))) { i++; continue; }

		if (inBlockComment) {
			// search for '*/'
			if (c == '*' && i + 1 < line.size() && line[i+1] == '/') {
				inBlockComment = false;
				i += 2; continue;
			}
			i++; continue;
		}

		// line comment //
		if (c == '/' && i + 1 < line.size() && line[i+1] == '/') {
			break; // ignore rest of line
		}
		// block comment start /*
		if (c == '/' && i + 1 < line.size() && line[i+1] == '*') {
			inBlockComment = true;
			i += 2; continue;
		}
		// two-char operators
		if (c == '&' && i + 1 < line.size() && line[i+1] == '&') { tokens.push_back({TokenType::AndAnd, "&&"}); i += 2; continue; }
		if (c == '|' && i + 1 < line.size() && line[i+1] == '|') { tokens.push_back({TokenType::OrOr, "||"}); i += 2; continue; }
		if (c == '=' && i + 1 < line.size() && line[i+1] == '=') { tokens.push_back({TokenType::EqualEqual, "=="}); i += 2; continue; }
		if (c == '!' && i + 1 < line.size() && line[i+1] == '=') { tokens.push_back({TokenType::BangEqual, "!="}); i += 2; continue; }
		if (c == '<' && i + 1 < line.size() && line[i+1] == '=') { tokens.push_back({TokenType::LessEqual, "<="}); i += 2; continue; }
		if (c == '>' && i + 1 < line.size() && line[i+1] == '=') { tokens.push_back({TokenType::GreaterEqual, ">="}); i += 2; continue; }

		// single-char punctuation/operators
		if (c == '(') { tokens.push_back({TokenType::LParen, "("}); i++; continue; }
		if (c == ')') { tokens.push_back({TokenType::RParen, ")"}); i++; continue; }
		if (c == '{') { tokens.push_back({TokenType::LBrace, "{"}); i++; continue; }
		if (c == '}') { tokens.push_back({TokenType::RBrace, "}"}); i++; continue; }
		if (c == '!') { tokens.push_back({TokenType::Bang, "!"}); i++; continue; }
		if (c == '<') { tokens.push_back({TokenType::Less, "<"}); i++; continue; }
		if (c == '>') { tokens.push_back({TokenType::Greater, ">"}); i++; continue; }
		if (c == '=') { tokens.push_back({TokenType::Equals, "="}); i++; continue; }
		if (c == ';') { tokens.push_back({TokenType::Semicolon, ";"}); i++; continue; }
		if (c == ',') { tokens.push_back({TokenType::Comma, ","}); i++; continue; }
		if (c == '+') { tokens.push_back({TokenType::Plus, "+"}); i++; continue; }

		if (c == '"') {
			// string literal
			size_t j = i + 1;
			while (j < line.size() && line[j] != '"') j++;
			std::string lex = (j < line.size()) ? line.substr(i+1, j-i-1) : line.substr(i+1);
			tokens.push_back({TokenType::StrLiteral, lex});
			i = (j < line.size()) ? j + 1 : line.size();
			continue;
		}

		if (std::isdigit(static_cast<unsigned char>(c))) {
			size_t j = i;
			while (j < line.size() && std::isdigit(static_cast<unsigned char>(line[j]))) j++;
			bool isFloat = false;
			if (j < line.size() && line[j] == '.' && (j + 1) < line.size() && std::isdigit(static_cast<unsigned char>(line[j+1]))) {
				isFloat = true;
				j++; 
				while (j < line.size() && std::isdigit(static_cast<unsigned char>(line[j]))) j++;
			}
			std::string num = line.substr(i, j-i);
			tokens.push_back({isFloat ? TokenType::FloatLiteral : TokenType::IntLiteral, num});
			i = j; continue;
		}

		if (isIdentStart(c)) {
			size_t j = i;
			while (j < line.size() && isIdentChar(line[j])) j++;
			std::string id = line.substr(i, j-i);
			if (id == "print") tokens.push_back({TokenType::KeywordPrint, id});
			else if (id == "read") tokens.push_back({TokenType::KeywordRead, id});
			else if (id == "int") tokens.push_back({TokenType::KeywordInt, id});
			else if (id == "str") tokens.push_back({TokenType::KeywordStr, id});
			else if (id == "float") tokens.push_back({TokenType::KeywordFloat, id});
			else if (id == "auto") tokens.push_back({TokenType::KeywordAuto, id});
			else if (id == "if") tokens.push_back({TokenType::KeywordIf, id});
			else if (id == "else") tokens.push_back({TokenType::KeywordElse, id});
			else if (id == "timeexec") tokens.push_back({TokenType::KeywordTimeExec, id});
			else tokens.push_back({TokenType::Identifier, id});
			i = j; continue;
		}

		tokens.push_back({TokenType::Unknown, std::string(1, c)});
		i++;
	}
	tokens.push_back({TokenType::EndOfInput, ""});
	return tokens;
}


