#include "Parser.h"
#include <sstream>

static bool match(const std::vector<Token>& t, size_t& i, TokenType type) {
	if (i < t.size() && t[i].type == type) { i++; return true; }
	return false;
}

static std::string tokDesc(const Token& tk) {
	std::ostringstream os;
	os << "token '" << tk.lexeme << "'";
	return os.str();
}

std::unique_ptr<Expr> Parser::parseTerm(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	if (i >= t.size()) { errorMsg = "unexpected end of input"; return nullptr; }
	if (t[i].type == TokenType::LParen) {
		i++;
		auto e = parseExpression(t, i, errorMsg);
		if (!e) return nullptr;
		if (i >= t.size() || t[i].type != TokenType::RParen) { errorMsg = "expected ')'"; return nullptr; }
		i++;
		return e;
	}
	if (t[i].type == TokenType::IntLiteral) { auto e = std::make_unique<IntLiteralExpr>(t[i].lexeme); i++; return e; }
	if (t[i].type == TokenType::FloatLiteral) { auto e = std::make_unique<FloatLiteralExpr>(t[i].lexeme); i++; return e; }
	if (t[i].type == TokenType::StrLiteral) { auto e = std::make_unique<StrLiteralExpr>(t[i].lexeme); i++; return e; }
	if (t[i].type == TokenType::Identifier) { auto e = std::make_unique<IdentifierExpr>(t[i].lexeme); i++; return e; }
	errorMsg = std::string("expected literal or identifier, got ") + tokDesc(t[i]);
	return nullptr;
}

std::unique_ptr<Expr> Parser::parseUnary(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	if (i < t.size() && t[i].type == TokenType::Bang) { i++; auto e = parseUnary(t, i, errorMsg); if (!e) return nullptr; return std::make_unique<UnaryExpr>("!", std::move(e)); }
	return parseTerm(t, i, errorMsg);
}

std::unique_ptr<Expr> Parser::parseAdditive(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	auto left = parseUnary(t, i, errorMsg);
	if (!left) return nullptr;
	while (i < t.size() && (t[i].type == TokenType::Plus)) {
		std::string op = t[i].lexeme; i++;
		auto right = parseUnary(t, i, errorMsg);
		if (!right) return nullptr;
		left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
	}
	return left;
}

std::unique_ptr<Expr> Parser::parseComparison(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	auto left = parseAdditive(t, i, errorMsg);
	if (!left) return nullptr;
	while (i < t.size() && (t[i].type == TokenType::Less || t[i].type == TokenType::LessEqual || t[i].type == TokenType::Greater || t[i].type == TokenType::GreaterEqual)) {
		std::string op = t[i].lexeme; i++;
		auto right = parseAdditive(t, i, errorMsg);
		if (!right) return nullptr;
		left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
	}
	return left;
}

std::unique_ptr<Expr> Parser::parseEquality(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	auto left = parseComparison(t, i, errorMsg);
	if (!left) return nullptr;
	while (i < t.size() && (t[i].type == TokenType::EqualEqual || t[i].type == TokenType::BangEqual)) {
		std::string op = t[i].lexeme; i++;
		auto right = parseComparison(t, i, errorMsg);
		if (!right) return nullptr;
		left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
	}
	return left;
}

std::unique_ptr<Expr> Parser::parseLogicalAnd(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	auto left = parseEquality(t, i, errorMsg);
	if (!left) return nullptr;
	while (i < t.size() && t[i].type == TokenType::AndAnd) {
		std::string op = t[i].lexeme; i++;
		auto right = parseEquality(t, i, errorMsg);
		if (!right) return nullptr;
		left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
	}
	return left;
}

std::unique_ptr<Expr> Parser::parseLogicalOr(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	auto left = parseLogicalAnd(t, i, errorMsg);
	if (!left) return nullptr;
	while (i < t.size() && t[i].type == TokenType::OrOr) {
		std::string op = t[i].lexeme; i++;
		auto right = parseLogicalAnd(t, i, errorMsg);
		if (!right) return nullptr;
		left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
	}
	return left;
}

std::unique_ptr<Expr> Parser::parseExpression(const std::vector<Token>& t, size_t& i, std::string& errorMsg) {
	return parseLogicalOr(t, i, errorMsg);
}

std::unique_ptr<Expr> Parser::parseExpr(const std::vector<Token>& tokens, size_t start, size_t end, std::string& errorMsg) {
	if (start >= end) { errorMsg = "empty expression"; return nullptr; }
	std::vector<Token> sub;
	sub.reserve(end - start + 1);
	for (size_t k = start; k < end; ++k) sub.push_back(tokens[k]);
	sub.push_back({TokenType::EndOfInput, ""});
	size_t idx = 0;
	auto e = parseExpression(sub, idx, errorMsg);
	if (!e) return nullptr;
	return e;
}

std::unique_ptr<Stmt> Parser::parseStatement(const std::vector<Token>& t, std::string& errorMsg) {
	size_t i = 0;
	// print("...");
	if (match(t, i, TokenType::KeywordPrint)) {
		if (!match(t, i, TokenType::LParen)) return nullptr;
		if (i >= t.size()) { errorMsg = "expected string literal in print(...)"; return nullptr; }
		if (t[i].type != TokenType::StrLiteral) { errorMsg = "print currently accepts only string literals"; return nullptr; }
		std::string content = t[i].lexeme; i++;
		if (!match(t, i, TokenType::RParen)) return nullptr;
		match(t, i, TokenType::Semicolon);
		return std::make_unique<PrintStmt>(content);
	}

	// read(ident);
	if (match(t, i, TokenType::KeywordRead)) {
		if (!match(t, i, TokenType::LParen)) return nullptr;
		if (i >= t.size() || t[i].type != TokenType::Identifier) { errorMsg = "expected identifier in read(...)"; return nullptr; }
		std::string name = t[i].lexeme; i++;
		if (!match(t, i, TokenType::RParen)) return nullptr;
		match(t, i, TokenType::Semicolon);
		return std::make_unique<ReadStmt>(name);
	}

	// var decl: int|str|float|auto name [= expr] ( , name [= expr] )* ;
	if (i < t.size() && (t[i].type == TokenType::KeywordInt || t[i].type == TokenType::KeywordStr || t[i].type == TokenType::KeywordFloat || t[i].type == TokenType::KeywordAuto)) {
		std::string typeName = "";
		if (t[i].type == TokenType::KeywordInt) typeName = "int";
		else if (t[i].type == TokenType::KeywordStr) typeName = "str";
		else if (t[i].type == TokenType::KeywordFloat) typeName = "float";
		else if (t[i].type == TokenType::KeywordAuto) typeName = "auto";
		i++;
		if (i >= t.size() || t[i].type != TokenType::Identifier) { errorMsg = "expected variable name after type"; return nullptr; }
		auto block = std::make_unique<BlockStmt>();
		while (true) {
			std::string varName = t[i].lexeme; i++;
			std::unique_ptr<Expr> initExpr;
			if (match(t, i, TokenType::Equals)) {
				initExpr = parseExpression(t, i, errorMsg);
				if (!initExpr) return nullptr;
			} else if (typeName == "auto") {
				errorMsg = "auto requires an initializer";
				return nullptr;
			}
			block->statements.push_back(std::make_unique<VarDeclStmt>(typeName, varName, std::move(initExpr)));
			if (match(t, i, TokenType::Comma)) {
				if (i >= t.size() || t[i].type != TokenType::Identifier) { errorMsg = "expected variable name after ','"; return nullptr; }
				continue;
			}
			break;
		}
		match(t, i, TokenType::Semicolon);
		return block;
	}

	// timeexec();
	if (i < t.size() && t[i].type == TokenType::KeywordTimeExec) {
		i++;
		if (!match(t, i, TokenType::LParen)) { errorMsg = "expected '(' after timeexec"; return nullptr; }
		if (!match(t, i, TokenType::RParen)) { errorMsg = "expected ')' in timeexec()"; return nullptr; }
		match(t, i, TokenType::Semicolon);
		if (!timeExecFlag) { errorMsg = "timeexec() unavailable"; return nullptr; }
		return std::make_unique<TimeExecStmt>(*timeExecFlag);
	}

	// assignment: ident = expr;
	if (i < t.size() && t[i].type == TokenType::Identifier) {
		std::string varName = t[i].lexeme; i++;
		if (!match(t, i, TokenType::Equals)) { errorMsg = "expected '=' after identifier"; return nullptr; }
		auto e = parseExpression(t, i, errorMsg);
		if (!e) return nullptr;
		match(t, i, TokenType::Semicolon);
		return std::make_unique<AssignStmt>(varName, std::move(e));
	}

	return nullptr;
}


