#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Token.h"
#include <memory>
#include <string>
#include <vector>

class Parser {
public:
	std::unique_ptr<Stmt> parseStatement(const std::vector<Token>& tokens, std::string& errorMsg);
	void setTimeExecFlag(bool* flagPtr) { timeExecFlag = flagPtr; }
	std::unique_ptr<Expr> parseExpr(const std::vector<Token>& tokens, size_t start, size_t end, std::string& errorMsg);
private:
	std::unique_ptr<Expr> parseExpression(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseLogicalOr(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseLogicalAnd(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseEquality(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseComparison(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseAdditive(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseUnary(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	std::unique_ptr<Expr> parseTerm(const std::vector<Token>& t, size_t& i, std::string& errorMsg);
	bool* timeExecFlag = nullptr;
};

#endif


