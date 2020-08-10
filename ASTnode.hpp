//
// Created by hanchong on 2020/7/20.
//

#ifndef BASIC_COMPILER_ASTNODE_HPP
#define BASIC_COMPILER_ASTNODE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "lexer.hpp"

//enum Operator {
//    _ADD, _MINUS, _MULTI, _DIVIDE, _AND, _OR, _EQUAL, _NOTEQUAL, _LESS, _LESSEQUAL, _GREATER, _GREEUQAL
//};

//class astNode {
//public:
//    virtual ~astNode() = default;
//};


class Atom {
public:
    int value = 0;
    std::string idname;
    Atom() {}
    Atom(const std::string &name): idname(name) {}
    Atom(const int &val): value(val) {}
};

class Expression {
public:
    TokenType op;
    std::shared_ptr<Expression> lhs = nullptr, rhs = nullptr;
    std::shared_ptr<Atom> atom = nullptr;
    Expression() {}
    Expression(TokenType _op, std::shared_ptr<Expression> _lhs, std::shared_ptr<Expression> _rhs): op(_op), lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}
    Expression(std::shared_ptr<Atom> _atom): atom(_atom) {}
    bool isatom() {
        if (atom) return true;
        else return false;
    }
};
class GotoStmt;
class InputStmt;
class VarDeclStmt;
class ExprStmt;
class IfStmt;
class ForStmt;
class ExitStmt;

class Statement {
public:
    std::shared_ptr<GotoStmt> goto_stmt = nullptr;
    std::shared_ptr<InputStmt> input_stmt = nullptr;
    std::shared_ptr<VarDeclStmt> vardecl_stmt = nullptr;
    std::shared_ptr<ExprStmt> expr_stmt = nullptr;
    std::shared_ptr<IfStmt> if_stmt = nullptr;
    std::shared_ptr<ForStmt> for_stmt = nullptr;
    std::shared_ptr<ExitStmt> exit_stmt = nullptr;
    std::shared_ptr<Atom> END = nullptr;

    Statement() {}
    Statement(std::shared_ptr<GotoStmt> p): goto_stmt(std::move(p)) {};
    Statement(std::shared_ptr<InputStmt> p): input_stmt(std::move(p)) {};
    Statement(std::shared_ptr<VarDeclStmt> p): vardecl_stmt(std::move(p)) {};
    Statement(std::shared_ptr<ExprStmt> p): expr_stmt(std::move(p)) {};
    Statement(std::shared_ptr<IfStmt> p): if_stmt(std::move(p)) {};
    Statement(std::shared_ptr<ForStmt> p): for_stmt(std::move(p)) {};
    Statement(std::shared_ptr<ExitStmt> p): exit_stmt(std::move(p)) {};
    Statement(std::shared_ptr<Atom> p): END(std::move(p)) {};
};

class GotoStmt {
public:
    int towhere = 0;
    GotoStmt(const int &num): towhere(num){}
};

class InputStmt {
public:
    std::vector<std::shared_ptr<Atom>> putthings;
    InputStmt(): putthings(0) {}
    void push(std::shared_ptr<Atom> obj) {
        putthings.push_back(obj);
    }
};

class VarDeclStmt {
public:
    std::shared_ptr<Atom> lvalue = nullptr;
    std::shared_ptr<Expression> rvalue = nullptr;
    VarDeclStmt(std::shared_ptr<Atom> obj1, std::shared_ptr<Expression> obj2): lvalue(std::move(obj1)), rvalue(std::move(obj2)){};
};

class ExprStmt {
public:
    std::shared_ptr<Atom> lvalue = nullptr;
    std::shared_ptr<Expression> rvalue = nullptr;
    ExprStmt(std::shared_ptr<Atom> obj1, std::shared_ptr<Expression> obj2): lvalue(std::move(obj1)), rvalue(std::move(obj2)){};
};

class IfStmt {
public:
    std::shared_ptr<Expression> ifexpr = nullptr;
    int towhere = 0;
    IfStmt(std::shared_ptr<Expression> obj1, int obj2): ifexpr(std::move(obj1)), towhere(obj2){}
};

class ForStmt {
public:
    std::shared_ptr<ExprStmt> forstmt = nullptr;
    std::shared_ptr<Expression> ifcontinue = nullptr;
    std::map<int, std::shared_ptr<Statement>> suites;
    std::vector<int> forlines;
    ForStmt(std::shared_ptr<ExprStmt> obj1, std::shared_ptr<Expression> obj2): forstmt(std::move(obj1)), ifcontinue(std::move(obj2)) {}
    void push(int line, std::shared_ptr<Statement> stmt) {
        suites[line] = stmt;
        forlines.push_back(line);
    }
};

class ExitStmt {
public:
    std::shared_ptr<Expression> exitexpr = nullptr;
    ExitStmt(std::shared_ptr<Expression> obj): exitexpr(std::move(obj)) {}
};

class Program {
public:
    std::map<int, std::shared_ptr<Statement>> stmts;
    std::vector<int> lines;
    void push(int line, std::shared_ptr<Statement> stmt) {
        stmts[line] = std::move(stmt);
        lines.push_back(line);
    }
};



#endif //BASIC_COMPILER_ASTNODE_HPP
