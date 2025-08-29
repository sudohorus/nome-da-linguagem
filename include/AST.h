#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using VarTable = std::unordered_map<std::string, std::string>;
using TypeTable = std::unordered_map<std::string, std::string>;

struct Expr {
	virtual ~Expr() = default;
	virtual std::string evaluate(VarTable& vars, TypeTable& types) = 0;
};

struct IntLiteralExpr : Expr {
	std::string value;
	explicit IntLiteralExpr(const std::string& v) : value(v) {}
	std::string evaluate(VarTable& /*vars*/, TypeTable& /*types*/) override { return value; }
};

struct StrLiteralExpr : Expr {
	std::string value;
	explicit StrLiteralExpr(const std::string& v) : value(v) {}
	std::string evaluate(VarTable& /*vars*/, TypeTable& /*types*/) override { return value; }
};

struct FloatLiteralExpr : Expr {
	std::string value;
	explicit FloatLiteralExpr(const std::string& v) : value(v) {}
	std::string evaluate(VarTable& /*vars*/, TypeTable& /*types*/) override { return value; }
};

struct IdentifierExpr : Expr {
	std::string name;
	explicit IdentifierExpr(const std::string& n) : name(n) {}
	std::string evaluate(VarTable& vars, TypeTable& /*types*/) override;
};

struct BinaryExpr : Expr {
	std::string op; // '+', '==', '!=', '<', '<=', '>', '>=', '&&', '||'
	std::unique_ptr<Expr> left;
	std::unique_ptr<Expr> right;
	BinaryExpr(std::string o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
		: op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
	std::string evaluate(VarTable& vars, TypeTable& types) override;
};

struct UnaryExpr : Expr {
	std::string op; // '!'
	std::unique_ptr<Expr> expr;
	UnaryExpr(std::string o, std::unique_ptr<Expr> e) : op(std::move(o)), expr(std::move(e)) {}
	std::string evaluate(VarTable& vars, TypeTable& types) override;
};

struct Stmt {
	virtual ~Stmt() = default;
	virtual bool execute(VarTable& vars, TypeTable& types) = 0; // returns false on fatal error
};

struct VarDeclStmt : Stmt {
	std::string typeName; // "int", "str", "float", "auto"
	std::string varName;
	std::unique_ptr<Expr> initExpr; 

	VarDeclStmt(const std::string& t, const std::string& n, std::unique_ptr<Expr> e)
		: typeName(t), varName(n), initExpr(std::move(e)) {}

	bool execute(VarTable& vars, TypeTable& types) override;
};

struct AssignStmt : Stmt {
	std::string varName;
	std::unique_ptr<Expr> expr;
	AssignStmt(const std::string& n, std::unique_ptr<Expr> e)
		: varName(n), expr(std::move(e)) {}
	bool execute(VarTable& vars, TypeTable& types) override;
};

struct PrintStmt : Stmt {
	std::string content; 
	explicit PrintStmt(const std::string& c) : content(c) {}
	bool execute(VarTable& vars, TypeTable& types) override;
};

struct ReadStmt : Stmt {
	std::string varName;
	explicit ReadStmt(const std::string& n) : varName(n) {}
	bool execute(VarTable& vars, TypeTable& types) override;
};

struct BlockStmt : Stmt {
	std::vector<std::unique_ptr<Stmt>> statements;
	bool execute(VarTable& vars, TypeTable& types) override;
};

struct IfStmt : Stmt {
	std::unique_ptr<Expr> condition;
	std::unique_ptr<BlockStmt> thenBlock;
	std::unique_ptr<BlockStmt> elseBlock; 
	IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> thenB, std::unique_ptr<BlockStmt> elseB)
		: condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}
	bool execute(VarTable& vars, TypeTable& types) override;
};

struct TimeExecStmt : Stmt {
	bool& flagRef;
	explicit TimeExecStmt(bool& f) : flagRef(f) {}
	bool execute(VarTable& /*vars*/, TypeTable& /*types*/) override { flagRef = true; return true; }
};

#endif


