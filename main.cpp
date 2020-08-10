#include <iostream>
#include "lexer.hpp"
#include "ASTnode.hpp"
#include "parser.hpp"
#include "CFG.hpp"


int main() {
    freopen("C:\\Users\\hanchong\\Desktop\\BASIC-Compiler-master\\testcases\\basic_test\\basic_4.txt", "r", stdin);
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
            std::cout << blocks[i].codes[j].co << ' ' << blocks[i].codes[j].rd << " " << blocks[i].codes[j].rs1 << " " << blocks[i].codes[j].rs2 << " " << blocks[i].codes[j].num << std::endl;
    gen_code();
    std::cout << "@00000000" << std::endl;
    for (int i = 0; i < instructions.size(); ++i) {
        int bin = 0xff;
        printf("%02X %02X %02X %02X ", instructions[i].first & bin, (instructions[i].first & (bin << 8)) >> 8, (instructions[i].first & (bin << 16)) >> 16, (instructions[i].first & (bin << 24)) >> 24);
    }
    //    std::cout << instructions[i].first << std::endl;
}
