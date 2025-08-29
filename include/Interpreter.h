#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <unordered_map>
#include <vector>
#include "AST.h"
#include "Lexer.h"
#include "Parser.h"

class Interpreter{
private:
    VarTable vars;
    TypeTable types;
    Lexer lexer;
    Parser parser;
    bool timeExecEnabled = false;
    struct ControlFrame { bool parentExec; bool thisExec; bool isIf; bool lastIfResult; };
    std::vector<ControlFrame> ctrlStack;
    bool pendingElseValid = false;
    bool pendingElseParentExec = false;
    bool pendingElseLastIf = false;
    int skipDepth = 0;

public:
    bool execute(const std::string& line); // returns false on fatal error
    bool isTimeExecEnabled() const { return timeExecEnabled; }
    bool isReadStatement(const std::string& line);
};

#endif


