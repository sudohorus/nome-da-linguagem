#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
	Identifier,
	IntLiteral,
	FloatLiteral,
	StrLiteral,
	KeywordPrint,
	KeywordRead,
	KeywordInt,
	KeywordStr,
	KeywordFloat,
	KeywordAuto,
	KeywordIf,
	KeywordElse,
	KeywordTimeExec,
	LParen,
	RParen,
	LBrace,
	RBrace,
	Bang,
	Less,
	Greater,
	LessEqual,
	GreaterEqual,
	EqualEqual,
	BangEqual,
	AndAnd,
	OrOr,
	Equals,
	Semicolon,
	Comma,
	Plus,
	EndOfInput,
	Unknown
};

struct Token {
	TokenType type;
	std::string lexeme;
};

#endif


