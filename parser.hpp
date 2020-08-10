//
// Created by hanchong on 2020/7/24.
//

#ifndef BASIC_COMPILER_PARSER_HPP
#define BASIC_COMPILER_PARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>
#include <bits/shared_ptr.h>
#include "lexer.hpp"
#include "ASTnode.hpp"

class Parser{
    std::vector<Token> tokens;
public:
    std::stack<int> for_line;
    std::vector<std::pair<int, int>> for_end_pair;
    Parser(std::vector<Token> obj): tokens(obj) {}
    std::shared_ptr<Expression> expr_parse();
    std::shared_ptr<Atom> number_parse() {
        if (tokens[0].type == NUMBER || tokens[0].type == LINE_NUMBER) {
            auto ret = std::make_shared<Atom>(tokens[0].value);
            tokens.erase(tokens.begin());
            return ret;
        }
        else return nullptr;
    }
    std::shared_ptr<Atom> identifier_parse() {
        if (tokens[0].type == ID) {
            auto ret = std::make_shared<Atom>(tokens[0].id_name);
            tokens.erase(tokens.begin());
            return ret;
        }
        else return nullptr;
    }
    std::shared_ptr<Expression> paren_parse() {
        if (tokens[0].type == LPAREN) tokens.erase(tokens.begin());
        auto expr = expr_parse();
        if (tokens[0].type == RPAREN) tokens.erase(tokens.begin());
        if (!expr) return nullptr;
        return expr;
    }
    std::shared_ptr<Expression> primary_parse() {
        auto ret = std::make_shared<Expression>();
        switch (tokens[0].type) {
            case NUMBER:
            case LINE_NUMBER:
                return std::make_shared<Expression>(number_parse());
            case ID:
                return std::make_shared<Expression>(identifier_parse());
            case LPAREN:
                return paren_parse();
            default:
                ret = nullptr;
                break;
        }
        // lack index
    }

    std::map<TokenType, int> precedence;
    void op_init() {
        precedence[OR] = 1;
        precedence[AND] = 2;
        precedence[EQUAL] = 3;
        precedence[NOTEQUAL] = 3;
        precedence[LESS] = 4;
        precedence[LESSE] = 4;
        precedence[GREAT] = 4;
        precedence[GREATE] = 4;
        precedence[ADD] = 5;
        precedence[SUB] = 5;
        precedence[MUL] = 6;
        precedence[DIV] = 6;
    }
    int get_precedence() {
        if (tokens[0].type == OR || tokens[0].type == AND || tokens[0].type == EQUAL || tokens[0].type == NOTEQUAL || tokens[0].type == LESS || tokens[0].type == LESSE || tokens[0].type == GREAT || tokens[0].type == GREATE || tokens[0].type == ADD || tokens[0].type == SUB || tokens[0].type == MUL || tokens[0].type == DIV)
            return precedence[tokens[0].type];
        else return -1;
    }
    std::shared_ptr<Expression> binoprhs_parse(int prece, std::shared_ptr<Expression> lhs) {
        while(1) {
            int curprece = get_precedence();
            if (curprece < prece) return lhs;
            TokenType op = tokens[0].type;
            tokens.erase(tokens.begin());
            auto rhs = primary_parse();
            if (!rhs) return nullptr;
            int nxtprece = get_precedence();
            if (curprece < nxtprece) {
                rhs = binoprhs_parse(curprece + 1, rhs);
                if (!rhs) return nullptr;
            }
            lhs = std::make_shared<Expression>(op, lhs, rhs);
        }
    }

    std::shared_ptr<InputStmt>input_parse() {
        if (tokens[0].type == INPUT) tokens.erase(tokens.begin());
        std::shared_ptr<InputStmt> _input(new InputStmt);
        _input->push(std::make_shared<Atom>(identifier_parse()->idname));
        while (tokens[0].type == COMMA) {
            tokens.erase(tokens.begin());
            _input->push(std::make_shared<Atom>(identifier_parse()->idname));
        }
        return _input;
    }
    std::shared_ptr<GotoStmt>goto_parse() {
        if (tokens[0].type == GOTO) tokens.erase(tokens.begin());
        auto towhere = number_parse();
        return std::make_shared<GotoStmt>(towhere->value);
    }
    std::shared_ptr<VarDeclStmt> vardecl_parse() {
        if (tokens[0].type == LET) tokens.erase(tokens.begin());
        auto lvalue = identifier_parse();
        if (tokens[0].type == ASSIGN) tokens.erase(tokens.begin());
        auto rvalue = expr_parse();
        return std::make_shared<VarDeclStmt>(std::make_shared<Atom>(lvalue->idname), rvalue);
    }
    std::shared_ptr<ExitStmt> exit_parse() {
        if (tokens[0].type == EXIT) tokens.erase(tokens.begin());
        return std::make_shared<ExitStmt>(std::move(expr_parse()));
    }
    std::shared_ptr<ExprStmt> exprstmt_parse() {
        auto lvalue = identifier_parse();
        if (tokens[0].type == ASSIGN) tokens.erase(tokens.begin());
        auto rvalue = expr_parse();
        return std::make_shared<ExprStmt>(std::make_shared<Atom>(lvalue->idname), rvalue);
    }

    std::shared_ptr<Statement> stmt_parse(int n);
    std::shared_ptr<IfStmt> if_parse() {
        if (tokens[0].type == IF) tokens.erase(tokens.begin());
        auto ifexpr = expr_parse();
        if (tokens[0].type == THEN) tokens.erase(tokens.begin());
        auto towhere = number_parse();
        return std::make_shared<IfStmt>(ifexpr, towhere->value);
    }
    std::shared_ptr<ForStmt> for_parse() {
        if (tokens[0].type == FOR) tokens.erase(tokens.begin());
        auto forexpr = exprstmt_parse();
        if (tokens[0].type == SEMICOLON) tokens.erase(tokens.begin());
        auto ifcontinue = expr_parse();
        auto forstmt = std::make_shared<ForStmt>(forexpr, ifcontinue);
        while (tokens[0].type != END_FOR) {
            auto line = number_parse();
            if (tokens[0].type == END_FOR) {
                int forline = for_line.top();
                for_line.pop();
                for_end_pair.push_back(std::make_pair(forline, line->value));
                auto ret = std::make_shared<Atom>("END_FOR");
                forstmt->push(line->value, std::make_shared<Statement>(ret));
                break;
            }
            auto stmtt = stmt_parse(line->value);
            if (stmtt)
                forstmt->push(line->value, stmtt);
//            if (stmtt->for_stmt != nullptr) {
//                for_line.push(line->value);
//                forstmt->push(line->value, stmtt);
//            }
        }
        tokens.erase(tokens.begin());
        return forstmt;
    }
    Program program_parse() {
        Program pro;
        while(!tokens.empty()) {
            auto line = number_parse();
            if (line == nullptr) return pro;
            else if (tokens[0].type == EOF_TOK) return pro;
            else {
                auto stmt = stmt_parse(line->value);
                if (stmt != nullptr && stmt->for_stmt != nullptr) for_line.push(line->value);
                if (stmt != nullptr) pro.push(line->value, std::move(stmt));
            }
        }
        return pro;
    }

};

std::shared_ptr<Expression> Parser::expr_parse() {
    auto lhs = primary_parse();
    if (!lhs) return nullptr;
    auto ret = binoprhs_parse(0, lhs);
    return ret;
}
std::shared_ptr<Statement> Parser::stmt_parse(int n) {
    switch(tokens[0].type) {
        case INPUT:
            return std::make_shared<Statement>(input_parse());
        case GOTO:
            return std::make_shared<Statement>(goto_parse());
        case EXIT:
            return std::make_shared<Statement>(exit_parse());
        case IF:
            return std::make_shared<Statement>(if_parse());
        case FOR:
            for_line.push(n);
            return std::make_shared<Statement>(for_parse());
        case LET:
            return std::make_shared<Statement>(vardecl_parse());
        case REM:
            tokens.erase(tokens.begin());
            return nullptr;
//        default:
//            return std::make_shared<Expression>(expr_parse());
    }
}
#endif //BASIC_COMPILER_PARSER_HPP
