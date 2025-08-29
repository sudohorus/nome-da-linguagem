#include "Interpreter.h"
#include "Error.h"
#include <iostream>
bool Interpreter::execute(const std::string& line) {
    auto tokensAll = lexer.tokenize(line);
    bool onlyEndAll = true;
    for (const auto& tk : tokensAll) { if (tk.type != TokenType::EndOfInput) { onlyEndAll = false; break; } }
    if (onlyEndAll) return true;
    std::vector<std::vector<Token>> parts;
    {
        std::vector<Token> cur;
        int depthPar = 0; int depthBrace = 0;
        for (size_t i = 0; i < tokensAll.size(); ++i) {
            const auto& tk = tokensAll[i];
            if (tk.type == TokenType::LParen) depthPar++;
            else if (tk.type == TokenType::RParen) depthPar--;
            else if (tk.type == TokenType::LBrace) depthBrace++;
            else if (tk.type == TokenType::RBrace) depthBrace--;
            cur.push_back(tk);
            if (tk.type == TokenType::Semicolon && depthPar == 0 && depthBrace == 0) {
                parts.push_back(cur);
                cur.clear();
            }
        }
        if (!cur.empty()) parts.push_back(cur);
    }

    auto computeCurrentExec = [&]() {
        bool exec = true;
        for (const auto& fr : ctrlStack) exec = exec && fr.thisExec;
        return exec;
    };

    auto truthy = [&](const std::string& s){
        if (s.empty()) return false;
        bool isNum = true, hasDot = false;
        for (char c : s) { if (c == '.') { if (hasDot) { isNum=false; break; } hasDot=true; } else if (!std::isdigit(static_cast<unsigned char>(c))) { isNum=false; break; } }
        if (isNum) { try { return std::stod(s) != 0.0; } catch (...) { return false; } }
        return true;
    };

    auto findMatchingRParen = [&](const std::vector<Token>& tokens, size_t lp) -> long long {
        int depth = 0;
        for (size_t i = lp; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::LParen) depth++;
            else if (tokens[i].type == TokenType::RParen) {
                depth--;
                if (depth == 0) return static_cast<long long>(i);
            }
        }
        return -1;
    };

    for (auto tokens : parts) {
        bool onlyEnd = true; for (auto& tk : tokens) if (tk.type != TokenType::EndOfInput) { onlyEnd=false; break; }
        if (onlyEnd) continue;

        if (!tokens.empty() && tokens[0].type == TokenType::RBrace) {
            if (!ctrlStack.empty()) {
                auto top = ctrlStack.back();
                ctrlStack.pop_back();
                if (top.isIf) {
                    bool anyTrue = top.lastIfResult || pendingElseLastIf;
                    pendingElseValid = true;
                    pendingElseParentExec = top.parentExec;
                    pendingElseLastIf = anyTrue;
                } else {
                    pendingElseValid = false;
                    pendingElseLastIf = false;
                }
            } else {
                pendingElseValid = false;
                pendingElseLastIf = false;
            }
            if (tokens.size() > 1 && tokens[1].type == TokenType::KeywordElse) {
                std::vector<Token> rest(tokens.begin() + 1, tokens.end());
                if (rest.size() > 1 && rest[1].type == TokenType::KeywordIf) {
                    size_t lp = 2; while (lp < rest.size() && rest[lp].type != TokenType::LParen) lp++;
                    if (lp >= rest.size()) return false;
                    long long rpLL = findMatchingRParen(rest, lp);
                    if (rpLL < 0) return false;
                    size_t rp = static_cast<size_t>(rpLL);
                    size_t lb = rp + 1; if (lb >= rest.size() || rest[lb].type != TokenType::LBrace) return false;
                    if (!pendingElseValid) return false;
                    bool parentExec = pendingElseParentExec;
                    bool allow = parentExec && (!pendingElseLastIf);
                    std::string err;
                    auto condExpr = parser.parseExpr(rest, lp + 1, rp, err);
                    if (!condExpr) return false;
                    bool cond = truthy(condExpr->evaluate(vars, types));
                    bool thisExec = allow && cond;
                    ControlFrame fr{parentExec, thisExec, true, thisExec};
                    ctrlStack.push_back(fr);
                    pendingElseValid = false;
                    continue;
                }
                size_t lb = 1; if (lb >= rest.size() || rest[lb].type != TokenType::LBrace) return false;
                if (!pendingElseValid) return false;
                bool parentExec = pendingElseParentExec;
                bool allow = parentExec && (!pendingElseLastIf);
                ControlFrame fr{parentExec, allow, false, false};
                ctrlStack.push_back(fr);
                pendingElseValid = false;
                continue;
            }
            continue;
        }

        // if (...){
        if (!tokens.empty() && tokens[0].type == TokenType::KeywordIf) {
            size_t lp = 1; while (lp < tokens.size() && tokens[lp].type != TokenType::LParen) lp++;
            if (lp >= tokens.size()) return false;
            long long rpLL = findMatchingRParen(tokens, lp);
            if (rpLL < 0) return false;
            size_t rp = static_cast<size_t>(rpLL);
            size_t lb = rp + 1; if (lb >= tokens.size() || tokens[lb].type != TokenType::LBrace) return false;
            bool parentExec = computeCurrentExec();
            std::string err;
            auto condExpr = parser.parseExpr(tokens, lp + 1, rp, err);
            if (!condExpr) return false;
            bool cond = truthy(condExpr->evaluate(vars, types));
            bool thisExec = parentExec && cond;
            ControlFrame fr{parentExec, thisExec, true, thisExec};
            ctrlStack.push_back(fr);
            continue;
        }

        // else / else if
        if (!tokens.empty() && tokens[0].type == TokenType::KeywordElse) {
            if (!pendingElseValid) return false;
            if (tokens.size() > 1 && tokens[1].type == TokenType::KeywordIf) {
                size_t lp = 2; while (lp < tokens.size() && tokens[lp].type != TokenType::LParen) lp++;
                if (lp >= tokens.size()) return false;
                long long rpLL = findMatchingRParen(tokens, lp);
                if (rpLL < 0) return false;
                size_t rp = static_cast<size_t>(rpLL);
                size_t lb = rp + 1; if (lb >= tokens.size() || tokens[lb].type != TokenType::LBrace) return false;
                bool parentExec = pendingElseParentExec;
                bool allow = parentExec && (!pendingElseLastIf);
                std::string err;
                auto condExpr = parser.parseExpr(tokens, lp + 1, rp, err);
                if (!condExpr) return false;
                bool cond = truthy(condExpr->evaluate(vars, types));
                bool thisExec = allow && cond;
                ControlFrame fr{parentExec, thisExec, true, thisExec};
                ctrlStack.push_back(fr);
                pendingElseValid = false;
                continue;
            }
            size_t lb = 1; if (lb >= tokens.size() || tokens[lb].type != TokenType::LBrace) return false;
            bool parentExec = pendingElseParentExec;
            bool allow = parentExec && (!pendingElseLastIf);
            ControlFrame fr{parentExec, allow, false, false};
            ctrlStack.push_back(fr);
            pendingElseValid = false;
            continue;
        }

        if (!computeCurrentExec()) continue;
        std::string errorMsg;
        parser.setTimeExecFlag(&timeExecEnabled);
        auto stmt = parser.parseStatement(tokens, errorMsg);
        if (!stmt) {
            if (!errorMsg.empty()) Err::parseError(errorMsg);
            else Err::error("command not found");
            return false;
        }
        if (!stmt->execute(vars, types)) return false;
    }

    return true;
}

bool Interpreter::isReadStatement(const std::string& line) {
    auto tokens = lexer.tokenize(line);
    for (const auto& tk : tokens) {
        if (tk.type == TokenType::EndOfInput) break;
        if (tk.type == TokenType::KeywordRead) return true;
        return false;
    }
    return false;
}
