#include <iostream>
#include <cstdio>
#include "lexer.hpp"
#include "ASTnode.hpp"
#include "parser.hpp"
#include "CFG.hpp"


int main() {
    freopen("C:\\Users\\hanchong\\Desktop\\BASIC-Compiler-master\\testcases\\op_test\\op_9.txt", "r", stdin);
    freopen("C:\\Users\\hanchong\\Desktop\\TEST\\test.txt", "w", stdout);
    Lexer lexer(std::cin);
    std::vector<Token> tokens;
    tokens = lexer.getTokens();
    //for (int i = 1; i < tokens.size(); ++i) tokens[i].print();
    Parser parser(tokens);
    parser.op_init();
    Program pro = parser.program_parse();
    CFG(pro, parser);
    loadstore();
    for(int i = 0; i < blocks.size(); ++i)
        for (int j = 0; j < blocks[i].codes.size(); ++j)
            //std::cout << blocks[i].codes[j].line << " " << blocks[i].codes[j].co << ' ' << blocks[i].codes[j].rd << " " << blocks[i].codes[j].rs1 << " " << blocks[i].codes[j].rs2 << " " << blocks[i].codes[j].num << std::endl;
    gen_code();
    std::cout << "@00000000" << std::endl;
    for (int i = 0; i < instructions.size(); ++i) {
        unsigned int bin = 0xff;
        unsigned int code = instructions[i].first;
        printf("%02X %02X %02X %02X ", code & bin, (code & (bin << 8)) >> 8, (code & (bin << 16)) >> 16, (code & (bin << 24)) >> 24);
    }
    //    std::cout << instructions[i].first << std::endl;
}
