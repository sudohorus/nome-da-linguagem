#include "AST.h"
#include <iostream>
#include <cctype>

static bool isAllDigits(const std::string& s) {
	if (s.empty()) return false;
	for (char c : s) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
	return true;
}

static bool isNumber(const std::string& s) {
	if (s.empty()) return false;
	bool hasDot = false;
	for (char c : s) {
		if (c == '.') { if (hasDot) return false; hasDot = true; continue; }
		if (!std::isdigit(static_cast<unsigned char>(c))) return false;
	}
	return true;
}

std::string IdentifierExpr::evaluate(VarTable& vars, TypeTable& /*types*/) {
	auto it = vars.find(name);
	return it != vars.end() ? it->second : std::string("undefined");
}

std::string UnaryExpr::evaluate(VarTable& vars, TypeTable& types) {
	std::string v = expr->evaluate(vars, types);
	auto truthy = [&](const std::string& s){
		if (s.empty()) return false;
		bool hasDot = false; bool isNum = true;
		for (char c : s) { if (c == '.') { if (hasDot) { isNum=false; break; } hasDot=true; } else if (!std::isdigit(static_cast<unsigned char>(c))) { isNum=false; break; } }
		if (isNum) { try { return std::stod(s) != 0.0; } catch (...) { return false; } }
		return true;
	};
	if (op == "!") return truthy(v) ? std::string("0") : std::string("1");
	return v;
}

std::string BinaryExpr::evaluate(VarTable& vars, TypeTable& types) {
	std::string l = left->evaluate(vars, types);
	std::string r = right->evaluate(vars, types);
	if (op == "+") {
		bool leftIsNum = isNumber(l);
		bool rightIsNum = isNumber(r);
		if (leftIsNum && rightIsNum) {
			if (l.find('.') != std::string::npos || r.find('.') != std::string::npos) {
				double a = 0, b = 0;
				try { a = std::stod(l); } catch (...) { a = 0; }
				try { b = std::stod(r); } catch (...) { b = 0; }
				return std::to_string(a + b);
			} else {
				long long a = 0, b = 0;
				try { a = std::stoll(l); } catch (...) { a = 0; }
				try { b = std::stoll(r); } catch (...) { b = 0; }
				return std::to_string(a + b);
			}
		}
		return l + r;
	}
	auto cmpAsNum = [&](auto f)->std::string{
		double a=0,b=0; try{a=std::stod(l);}catch(...){a=0;} try{b=std::stod(r);}catch(...){b=0;}
		return f(a,b) ? std::string("1") : std::string("0");
	};
	auto cmpAsStr = [&](auto f)->std::string{
		return f(l,r) ? std::string("1") : std::string("0");
	};
	bool bothNum = isNumber(l) && isNumber(r);
	if (op == "==") return bothNum ? cmpAsNum([](double a,double b){return a==b;}) : cmpAsStr([](const std::string&a,const std::string&b){return a==b;});
	if (op == "!=") return bothNum ? cmpAsNum([](double a,double b){return a!=b;}) : cmpAsStr([](const std::string&a,const std::string&b){return a!=b;});
	if (op == "<")  return bothNum ? cmpAsNum([](double a,double b){return a<b;})  : cmpAsStr([](const std::string&a,const std::string&b){return a<b;});
	if (op == "<=") return bothNum ? cmpAsNum([](double a,double b){return a<=b;}) : cmpAsStr([](const std::string&a,const std::string&b){return a<=b;});
	if (op == ">")  return bothNum ? cmpAsNum([](double a,double b){return a>b;})  : cmpAsStr([](const std::string&a,const std::string&b){return a>b;});
	if (op == ">=") return bothNum ? cmpAsNum([](double a,double b){return a>=b;}) : cmpAsStr([](const std::string&a,const std::string&b){return a>=b;});
	if (op == "&&" || op == "||") {
		auto truthy = [&](const std::string& s){
			if (s.empty()) return false;
			bool hasDot=false; bool isNum=true;
			for (char c : s) { if (c=='.'){ if(hasDot){isNum=false; break;} hasDot=true;} else if(!std::isdigit(static_cast<unsigned char>(c))) { isNum=false; break; } }
			if (isNum) { try { return std::stod(s)!=0.0; } catch(...) { return false; } }
			return true;
		};
		bool L = truthy(l), R = truthy(r);
		bool res = (op == "&&") ? (L && R) : (L || R);
		return res ? std::string("1") : std::string("0");
	}
	return "";
}

bool VarDeclStmt::execute(VarTable& vars, TypeTable& types) {
	std::string value;
	if (initExpr) {
		value = initExpr->evaluate(vars, types);
		if (typeName == "int" && !isAllDigits(value)) {
			std::cout << "[fatal] type mismatch: cannot assign string to int" << std::endl;
			return false;
		}
		if (typeName == "float" && !isNumber(value)) {
			std::cout << "[fatal] type mismatch: cannot assign non-number to float" << std::endl;
			return false;
		}
	} else {
		if (typeName == "int") value = "0";
		else if (typeName == "float") value = "0.0";
		else value = std::string("");
	}
	if (typeName == "auto") {
		if (isNumber(value)) {
			if (value.find('.') != std::string::npos) types[varName] = "float"; else types[varName] = "int";
		} else {
			types[varName] = "str";
		}
	} else {
		types[varName] = typeName;
	}
	vars[varName] = value;
	return true;
}

bool PrintStmt::execute(VarTable& vars, TypeTable& /*types*/) {
	std::string out = content;
	size_t pos = 0;
	while ((pos = out.find('{', pos)) != std::string::npos) {
		size_t end = out.find('}', pos);
		if (end == std::string::npos) break;
		std::string var = out.substr(pos+1, end-pos-1);
		auto it = vars.find(var);
		std::string val = (it != vars.end()) ? it->second : std::string("undefined");
		out.replace(pos, end-pos+1, val);
		pos += val.size();
	}
	std::cout << out << std::endl;
	return true;
}

bool ReadStmt::execute(VarTable& vars, TypeTable& types) {
	if (!types.count(varName)) {
		std::cout << "[error] undeclared variable: " << varName << std::endl;
		return false;
	}
	std::string input;
	std::cout << varName << ": ";
	std::cout.flush();
	std::getline(std::cin, input);
	if (types[varName] == "int") {
		try {
			int v = std::stoi(input);
			vars[varName] = std::to_string(v);
		} catch (...) {
			std::cout << "[error] invalid value for int" << std::endl;
			return false;
		}
	} else if (types[varName] == "float") {
		try {
			double v = std::stod(input);
			vars[varName] = std::to_string(v);
		} catch (...) {
			std::cout << "[error] invalid value for float" << std::endl;
			return false;
		}
	} else {
		vars[varName] = input;
	}
	return true;
}

bool AssignStmt::execute(VarTable& vars, TypeTable& types) {
	if (!types.count(varName)) {
		std::cout << "[error] assignment to undeclared variable: " << varName << std::endl;
		return false;
	}
	std::string v = expr->evaluate(vars, types);
	if (types[varName] == "int") {
		if (!isAllDigits(v)) {
			std::cout << "[fatal] type mismatch: cannot assign string to int" << std::endl;
			return false;
		}
	} else if (types[varName] == "float") {
		if (!isNumber(v)) {
			std::cout << "[fatal] type mismatch: cannot assign non-number to float" << std::endl;
			return false;
		}
	}
	vars[varName] = v;
	return true;
}

bool BlockStmt::execute(VarTable& vars, TypeTable& types) {
	for (auto& st : statements) {
		if (!st->execute(vars, types)) return false;
	}
	return true;
}



