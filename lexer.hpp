//
// Created by hanchong on 2020/7/20.
//

#ifndef BASIC_COMPILER_LEXER_HPP
#define BASIC_COMPILER_LEXER_HPP

#include <iostream>
#include <vector>
#include <string>

enum TokenType {
    ID = 0, NUMBER = 1, LET = 2, INPUT = 4, EXIT = 5, GOTO = 6, IF = 7, THEN = 8,
    FOR = 9, END_FOR = 10, ASSIGN = 11, ADD = 12, SUB = 13, MUL = 14, DIV = 15,
    AND = 16, OR = 17, EQUAL = 18, NOTEQUAL = 19, LESS = 20, GREAT = 21, LESSE = 22, GREATE = 23,
    LINE_NUMBER = 24, LINE_BREAK = 25, EOF_TOK = 26, LPAREN = 27, RPAREN = 28, LSQUARE = 29, RSQUARE = 30,
    SEMICOLON = 31, COMMA = 32, REM = 33
};

class Token {
public:
    TokenType type;
    std::string id_name = "";
    int value = 0;
    Token();
    Token (TokenType obj): type(obj) {}
    Token (TokenType obj1, std::string obj2): type(obj1), id_name(obj2) {}
    Token (TokenType obj1, int obj2): type(obj1), value(obj2) {}
    void print() {
        std::cout << type << " ";
        if (type == ID) std::cout << id_name << " ";
        if (type == NUMBER || type == LINE_NUMBER) std::cout << value << " ";
        std::cout << std::endl;
    }
};

class Lexer {
private:
    std::istream &is;
    std::vector<Token> Tokens;
    char lastchar = ' ';
public:
    Lexer(std::istream &_is):is(_is) {}
    Token getnextToken() {
        lastchar = ' ';
        std::string tmpname;
        std::string tmpvalue;
        while (isspace(lastchar)) {
            lastchar = getchar();
//            if (lastchar == '\n' || lastchar == '\r')
//                return Token(LINE_BREAK);
        }
        if (lastchar == EOF) return Token(EOF_TOK);
        if (isalpha(lastchar)) {
            tmpname = lastchar;
            while (isalpha((lastchar = getchar())))
                tmpname += lastchar;
            std::cin.putback(lastchar);
//            if (lastchar == ']' || lastchar == ')' || lastchar == ',' || lastchar == ';' || lastchar == EOF)
//                std::cin.unget();
            if (tmpname == "LET") return Token(LET);
            else if (tmpname == "INPUT") return Token(INPUT);
            else if (tmpname == "EXIT") return Token(EXIT);
            else if (tmpname == "GOTO") return Token(GOTO);
            else if (tmpname == "IF") return Token(IF);
            else if (tmpname == "THEN") return Token(THEN);
            else if (tmpname == "FOR") return Token(FOR);
            else if (tmpname == "END") return Token(END_FOR);
            else if (tmpname == "REM") {
                do lastchar = getchar();
                while (lastchar != EOF && lastchar != '\n' && lastchar != '\r');
                if (lastchar != EOF) return Token(REM);
                else {
                    std::cin.putback(lastchar);
                    return Token(REM);
                }
            }
            else return Token(ID, tmpname);
        }
        else if (isdigit(lastchar)) {
            do {
                tmpvalue += lastchar;
                lastchar = getchar();
            } while (isdigit(lastchar));
            std::cin.putback(lastchar);
//            if (lastchar == ']' || lastchar == ')' || lastchar == ',' || lastchar == ';' || lastchar == EOF)
//                std::cin.unget();
            return Token(NUMBER, stoi(tmpvalue));
        }
        else if (lastchar == '=') {
            lastchar = getchar();
            if (isspace(lastchar)) return Token(ASSIGN);
            else if (lastchar == '=') return Token(EQUAL);
        }
        else if (lastchar == '+') return Token(ADD);
        else if (lastchar == '-') {
            lastchar = getchar();
            if (isdigit(lastchar)) {
                tmpvalue = "-";
                do {
                    tmpvalue += lastchar;
                    lastchar = getchar();
                } while (isdigit(lastchar));
                std::cin.putback(lastchar);
                return Token(NUMBER, stoi(tmpvalue));
            }
            else {
                std::cin.putback(lastchar);
                return Token(SUB);
            }
        }
        else if (lastchar == '*') return Token(MUL);
        else if (lastchar == '/') return Token(DIV);
        else if (lastchar == '&') {
            lastchar = getchar();
            if (lastchar == '&') return Token(AND);
        }
        else if (lastchar == '|') {
            lastchar = getchar();
            if (lastchar == '|') return Token(OR);
        }
        else if (lastchar == '!') {
            lastchar = getchar();
            if (lastchar == '=') return Token(NOTEQUAL);
        }
        else if (lastchar == '<') {
            lastchar = getchar();
            if (isspace(lastchar)) return Token(LESS);
            else if (lastchar == '=') return Token(LESSE);
        }
        else if (lastchar == '>') {
            lastchar = getchar();
            if (isspace(lastchar)) return Token(GREAT);
            else if (lastchar == '=') return Token(GREATE);
        }
        else if (lastchar == '(') return Token(LPAREN);
        else if (lastchar == '[') return Token(LSQUARE);
        else if (lastchar == ')') {
            lastchar = ' ';
            return Token(RPAREN);
        }
        else if (lastchar == ']') {
            lastchar = ' ';
            return Token(RSQUARE);
        }
        else if (lastchar == ';') {
            lastchar = ' ';
            return Token(SEMICOLON);
        }
        else if (lastchar == ',') {
            lastchar = ' ';
            return Token(COMMA);
        }
    }
    std::vector<Token> getTokens() {
        while (1) {
            Token nxt = getnextToken();
            if (nxt.type == END_FOR) {
                Tokens.push_back(nxt);
                for (int i = 1; i <= 4; ++i) getchar();
            }
//            else if (nxt.type == LINE_BREAK) {
//                char lastchar = ' ';
//                std::string tmpvalue;
//                while (isspace(lastchar)) lastchar = getchar();
//                if (isdigit(lastchar)) {
//                    do {
//                        tmpvalue += lastchar;
//                        lastchar = getchar();
//                    } while (isdigit(lastchar));
//                    Tokens.push_back(Token(LINE_NUMBER, stoi(tmpvalue)));
//                }
//            }
            else if (nxt.type == EOF_TOK) {
                Tokens.push_back(nxt);
                break;
            }
            else Tokens.push_back(nxt);
        }
        return Tokens;
    }

};


#endif //BASIC_COMPILER_LEXER_HPP
