//
// Created by hanchong on 2020/7/28.
//

#ifndef BASIC_COMPILER_CFG_HPP
#define BASIC_COMPILER_CFG_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "parser.hpp"

    unsigned char mem[0x20000];
    int reg[35];

    enum command {
        LOAD = 0, STORE = 1, ADDI = 2, SUBI = 3, MULI = 4, DIVI = 5, ANDI = 6, ORI = 7, JAL = 8,
        EQ = 9, NEQ = 10, LT = 11, LTE = 12, GR = 13, GRE = 14, BEQ = 15, BNE = 16
    };

    class tmpcode{
    public:
        command co;
        std::string rd = "", rs1 = "", rs2 = "";
        int num = 0;
        int line = 0;
        int rsrs1 = 0, rsrs2 = 0, addr = 0;
        tmpcode() {}
        tmpcode(command obj1, int obj2, int where): co(obj1), num(obj2), line(where) {}
        tmpcode(command obj1, std::string obj2, int where): co(obj1), rd(obj2), line(where) {}
        tmpcode(command obj1, std::string obj2, int obj3, int where): co(obj1), rd(obj2), num(obj3), line(where) {}
        tmpcode(command obj1, std::string obj2, std::string obj3, int obj4, int where): co(obj1), rs1(obj2), rs2(obj3), num(obj4), line(where) {}
        tmpcode(command obj1, std::string obj2, std::string obj3, std::string obj4, int where): co(obj1), rd(obj2), rs1(obj3), rs2(obj4), line(where) {}
        tmpcode init(command obj1, std::string obj2, std::string obj3, std::string obj4, int where) {
            co = obj1;
            rd = obj2;
            rs1 = obj3;
            rs2 = obj4;
            line = where;
            return *this;
        }
    };

    class block{
    public:
        std::vector<tmpcode> codes;
        block() {}
    };

    std::vector<block> blocks;
    int tmpcount = 0;
    std::string exprcfg(std::shared_ptr<Expression> curexpr, int line) {
        if (curexpr->isatom()) {
            if (curexpr->atom->idname != "") return curexpr->atom->idname;
            else return std::to_string(curexpr->atom->value);
        }
        else {
            std::string tmpl = exprcfg(curexpr->lhs, line);
            std::string tmpr = exprcfg(curexpr->rhs, line);
            std::string tmp = "tmp";
            tmp += std::to_string(tmpcount);
            ++tmpcount;
            tmpcode tmpp;
            switch (curexpr->op) {
                case OR:
                    tmpp.init(ORI, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case AND:
                    tmpp.init(ANDI, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case EQUAL:
                    tmpp.init(EQ, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case NOTEQUAL:
                    tmpp.init(NEQ, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case LESS:
                    tmpp.init(LT, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case LESSE:
                    tmpp.init(LTE, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case GREAT:
                    tmpp.init(GR, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case GREATE:
                    tmpp.init(GRE, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case ADD:
                    tmpp.init(ADDI, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case SUB:
                    tmpp.init(SUBI, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case MUL:
                    tmpp.init(MULI, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
                case DIV:
                    tmpp.init(DIVI, tmp, tmpl, tmpr, line);
                    blocks.back().codes.push_back(tmpp);
                    break;
            }
            return tmp;
        }
    }
    void forcfg(std::shared_ptr<ForStmt> obj, Program &pro, Parser &par, int line) {
        block newblock;
        blocks.push_back(newblock);
        std::string forexpr = exprcfg(obj->ifcontinue, line);
        int afterend = 0;
        int forbegin = 0;
        for (int i = 0; i < par.for_end_pair.size(); ++i)
            if (par.for_end_pair[i].first == line)
                afterend = par.for_end_pair[i].second + 1;
        tmpcode tmp(BEQ, forexpr, "0", afterend, line);
        blocks.back().codes.push_back(tmp);
        for (int i = 0; i < obj->forlines.size(); ++i) {
            auto tmpline = obj->suites[obj->forlines[i]];
            if (tmpline->goto_stmt != nullptr) {
                if (tmpline->goto_stmt->towhere == line) {
                    tmpcode tmp(JAL, afterend - 1, obj->forlines[i]);
                    blocks.back().codes.push_back(tmp);
                    block newblock;
                    blocks.push_back(newblock);
                }
                else {
                    tmpcode tmp(JAL, tmpline->goto_stmt->towhere, obj->forlines[i]);
                    blocks.back().codes.push_back(tmp);
                    block newblock;
                    blocks.push_back(newblock);
                }
            }
            else if (tmpline->input_stmt != nullptr) {
                for (int j = 0; j < tmpline->input_stmt->putthings.size(); ++j) {
                    int put = 0;
                    std::cin >> put;
                    tmpcode tmp(STORE, tmpline->input_stmt->putthings[j]->idname, put, obj->forlines[i]);
                    blocks.back().codes.push_back(tmp);
                }
            }
            else if (tmpline->vardecl_stmt != nullptr) {
                std::string lval = tmpline->vardecl_stmt->lvalue->idname;
                std::string rval = exprcfg(tmpline->vardecl_stmt->rvalue, obj->forlines[i]);
                tmpcode tmp(ADDI, lval, rval, "0", obj->forlines[i]);
                blocks.back().codes.push_back(tmp);
            }
            else if (tmpline->expr_stmt != nullptr) {
                std::string lval = tmpline->vardecl_stmt->lvalue->idname;
                std::string rval = exprcfg(tmpline->vardecl_stmt->rvalue, obj->forlines[i]);
                tmpcode tmp(ADDI, lval, rval, "0", obj->forlines[i]);
                blocks.back().codes.push_back(tmp);
            }
            else if (tmpline->if_stmt != nullptr) {
                std::string ifexpr = exprcfg(tmpline->if_stmt->ifexpr, obj->forlines[i]);
                int towhere = tmpline->if_stmt->towhere;
                if (towhere == line) {
                    tmpcode tmp(BNE, ifexpr, "0", afterend - 1, obj->forlines[i]);
                    blocks.back().codes.push_back(tmp);
                    block newblock;
                    blocks.push_back(newblock);
                }
                else {
                    tmpcode tmp(BNE, ifexpr, "0", towhere, obj->forlines[i]);
                    blocks.back().codes.push_back(tmp);
                    block newblock;
                    blocks.push_back(newblock);
                }
            }
            else if (tmpline->for_stmt != nullptr) {
                forcfg(tmpline->for_stmt, pro, par, obj->forlines[i]);
            }
            else if (tmpline->exit_stmt != nullptr) {
                std::string exitvalue = exprcfg(tmpline->exit_stmt->exitexpr, obj->forlines[i]);
                std::string tmpreg = "x10";
                tmpcode tmp(ADDI, tmpreg, exitvalue, "0", obj->forlines[i]);
                blocks.back().codes.push_back(tmp);
            }
            else if (tmpline->END != nullptr) {
                std::string lval = obj->forstmt->lvalue->idname;
                std::string rval = exprcfg(obj->forstmt->rvalue, obj->forlines[i]);
                tmpcode tmp1(ADDI, lval, rval, "0", obj->forlines[i]);
                blocks.back().codes.push_back(tmp1);
                forexpr = exprcfg(obj->ifcontinue, obj->forlines[i]);
                tmpcode tmp2(BEQ, forexpr, "0", afterend, obj->forlines[i]);
                blocks.back().codes.push_back(tmp2);
                for (int j = 0; j < par.for_end_pair.size(); ++j)
                    if (par.for_end_pair[j].second == obj->forlines[i])
                        forbegin = par.for_end_pair[j].first + 1;
                tmpcode tmp3(JAL, forbegin, obj->forlines[i]);
                blocks.back().codes.push_back(tmp3);
            }
        }
    }
    void CFG(Program &proo, Parser &parse) {
        block newb;
        blocks.push_back(newb);
        for (int i = 0; i < proo.lines.size(); ++i) {
            auto tmpline = proo.stmts[proo.lines[i]];
            if (tmpline->goto_stmt != nullptr) {
                tmpcode tmp(JAL, tmpline->goto_stmt->towhere, proo.lines[i]);
                blocks.back().codes.push_back(tmp);
                block newblock;
                blocks.push_back(newblock);
            }
            else if (tmpline->input_stmt != nullptr) {
                for (int j = 0; j < tmpline->input_stmt->putthings.size(); ++j) {
                    int put = 1;
                    if (j == 0) put = 8;
                    if (j == 1) put = 10;
//                    if (j == 2) put = 2;
//                    if (j == 3) put = 3;
                    //std::cin >> put;
                    tmpcode tmp(STORE, tmpline->input_stmt->putthings[j]->idname, put, proo.lines[i]);
                    blocks.back().codes.push_back(tmp);
                }
            }
            else if (tmpline->vardecl_stmt != nullptr) {
                std::string lval = tmpline->vardecl_stmt->lvalue->idname;
                std::string rval = exprcfg(tmpline->vardecl_stmt->rvalue, proo.lines[i]);
                tmpcode tmp(ADDI, lval, rval, "0", proo.lines[i]);
                blocks.back().codes.push_back(tmp);
            }
            else if (tmpline->expr_stmt != nullptr) {
                std::string lval = tmpline->vardecl_stmt->lvalue->idname;
                std::string rval = exprcfg(tmpline->vardecl_stmt->rvalue, proo.lines[i]);
                tmpcode tmp(ADDI, lval, rval, "0", proo.lines[i]);
                blocks.back().codes.push_back(tmp);
            }
            else if (tmpline->if_stmt != nullptr) {
                std::string ifexpr = exprcfg(tmpline->if_stmt->ifexpr, proo.lines[i]);
                int towhere = tmpline->if_stmt->towhere;
                tmpcode tmp(BNE, ifexpr, "0", towhere, proo.lines[i]);
                blocks.back().codes.push_back(tmp);
                block newblock;
                blocks.push_back(newblock);
            }
            else if (tmpline->for_stmt != nullptr) {
                forcfg(tmpline->for_stmt, proo, parse, proo.lines[i]);
            }
            else if (tmpline->exit_stmt != nullptr) {
                std::string exitvalue = exprcfg(tmpline->exit_stmt->exitexpr, proo.lines[i]);
                std::string tmpreg = "x10";
                tmpcode tmp(ADDI, tmpreg, exitvalue, "0", proo.lines[i]);
                blocks.back().codes.push_back(tmp);
            }
            if (i == proo.lines.size() - 1 && tmpline->exit_stmt == nullptr) {
                std::string tmpreg = "x10";
                tmpcode tmp(ADDI, tmpreg, "0", "0", proo.lines[i]);
                blocks.back().codes.push_back(tmp);
                break;
            }
        }
    }

    std::vector <std::pair<int, tmpcode>> instructions;
    int addr = 0, idaddr = 0x10000;
    std::map<int, int> line_addr;
    std::map<std::string, int> id_addr;
    std::map<std::string, int> name_regi;
    std::map<std::string, bool> name_used;
    bool regiused[35] = {false};
    bool iftmp(std::string regname) {
        std::string ret = regname.substr(0, 3);
        return (ret == "tmp");
    }
    bool ifnum(std::string regname) {
        for (int i = 0; i < regname.size(); ++i) {
            if (std::isdigit(regname[i])) continue;
            else return false;
        }
        return true;
    }
    void loadstore() {
        for(int i = 0; i < blocks.size(); ++i) for (int j = 0; j < blocks[i].codes.size(); ++j) {
            int count = 0;
            tmpcode tmp = blocks[i].codes[j];
            switch (tmp.co) {
                case LOAD:
                case STORE:
                    break;
                default:
                    if (!iftmp(tmp.rd) && !ifnum(tmp.rd) && tmp.rd != "x10") {
                        tmpcode t(STORE, tmp.rd, tmp.line);
                        blocks[i].codes.insert(blocks[i].codes.begin() + j + 1, t);
                        ++count;
                    }
                    if (!ifnum(tmp.rs1) && !iftmp(tmp.rs1)) {
                        tmpcode t1(LOAD, tmp.rs1, tmp.line);
                        blocks[i].codes.insert(blocks[i].codes.begin() + j, t1);
                        ++count;
                    }
                    if (!ifnum(tmp.rs2) && !iftmp(tmp.rs2) && (tmp.rs2 != tmp.rs1)) {
                        tmpcode t2(LOAD, tmp.rs2, tmp.line);
                        blocks[i].codes.insert(blocks[i].codes.begin() + j, t2);
                        ++count;
                    }
                    j += count;
                    break;
            }
        }
    }
    int get_num(int inst, int l, int r) {
        int ret = ((inst >> l) & (1 << (r - l + 1)) - 1);
        return ret;
    }
    int lw(int rd, int rs1, int offset) {
        addr += 4;
        int ret = offset;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 2;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 3;
        return ret;
    }
    int sw(int rs1, int rs2, int offset) {
        addr += 4;
        int ret = get_num(offset, 5, 11);
        ret <<= 5;
        ret += rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 2;
        ret <<= 5;
        ret += get_num(offset, 0, 4);
        ret <<= 7;
        ret += 35;
        return ret;
    }
    int addi(int rd, int rs1, int imm) {
        addr += 4;
        int ret = imm;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 19;
        return ret;
    }
    int add(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int sub(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = 32;
        ret <<= 5;
        ret += rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int mul(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = 1;
        ret <<= 5;
        ret += rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int div(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = 2;
        ret <<= 5;
        ret += rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int andi(int rd, int rs1, int imm) {
        addr += 4;
        int ret = imm;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 7;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 19;
        return ret;
    }
    int _and(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 7;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int ori(int rd, int rs1, int imm) {
        addr += 4;
        int ret = imm;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 6;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 19;
        return ret;
    }
    int _or(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 6;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int xori(int rd, int rs1, int imm) {
        addr += 4;
        int ret = imm;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 4;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 19;
        return ret;
    }
    int _xor(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 4;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int slt(int rd, int rs1, int rs2) {
        addr += 4;
        int ret = rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 2;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 51;
        return ret;
    }
    int slti(int rd, int rs1, int imm) {
        addr += 4;
        int ret = imm;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 2;
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 19;
        return ret;
    }
    int beq(int rs1, int rs2, int offset) {
        int ret = get_num(offset, 12, 12);
        ret <<= 6;
        ret += get_num(offset, 5, 10);
        ret <<= 5;
        ret += rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret <<= 4;
        ret += get_num(offset, 1, 4);
        ret <<= 1;
        ret += get_num(offset, 11, 11);
        ret <<= 7;
        ret += 99;
        return ret;
    }
    int bne(int rs1, int rs2, int offset) {
        int ret = get_num(offset, 12, 12);
        ret <<= 6;
        ret += get_num(offset, 5, 10);
        ret <<= 5;
        ret += rs2;
        ret <<= 5;
        ret += rs1;
        ret <<= 3;
        ret += 1;
        ret <<= 4;
        ret += get_num(offset, 1, 4);
        ret <<= 1;
        ret += get_num(offset, 11, 11);
        ret <<= 7;
        ret += 99;
        return ret;
    }
    int jal(int rd, int offset) {
//        int ret = ((offset & 0x7fe) << 20) | (((offset>> 20) & 1) << 31) | (((offset >> 11) & 1) << 20) | (((offset >> 12) & 0xff) << 12) | 0b1101111;
//        return ret;
        int ret = get_num(offset, 20, 20);
        ret <<= 10;
        ret += get_num(offset, 1, 10);
        ret <<= 1;
        ret += get_num(offset, 11, 11);
        ret <<= 8;
        ret += get_num(offset, 12, 19);
        ret <<= 5;
        ret += rd;
        ret <<= 7;
        ret += 111;
        return ret;
    }
    int lui(int rd, int imm) {
        addr += 4;
        int ret = ((imm >> 12) << 12) | (rd << 7) | 0b0110111;
//        int ret = get_num(imm, 0, 19);
//        ret <<= 5;
//        ret += rd;
//        ret <<= 7;
//        ret += 55;
        return ret;
    }

    //变量-load/store，立即数+I
    void gen_code() {
        name_regi["0"] = 0;
        int rd = 0;
        for(int i = 0; i < blocks.size(); ++i) for (int j = 0; j < blocks[i].codes.size(); ++j) {
            tmpcode cur = blocks[i].codes[j];
            switch (cur.co) {
                case STORE:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if(id_addr.count(cur.rd) == 0) {
                        if(name_regi.count(cur.rd) == 0) {
                            instructions.push_back(std::make_pair(addi(1, 0, cur.num), cur));
                            instructions.push_back(std::make_pair(lui(2, idaddr), cur));
                            instructions.push_back(std::make_pair(sw(2, 1, idaddr), cur));
                            id_addr[cur.rd] = idaddr;
                            idaddr += 4;
                        }
                        else {
                            instructions.push_back(std::make_pair(lui(2, idaddr), cur));
                            instructions.push_back(std::make_pair(sw(2, name_regi[cur.rd], idaddr), cur));
                            id_addr[cur.rd] = idaddr;
                            idaddr += 4;
                        }
                    }
                    else {
                        instructions.push_back(std::make_pair(lui(2, id_addr[cur.rd]), cur));
                        instructions.push_back(std::make_pair(sw(2, name_regi[cur.rd], id_addr[cur.rd]), cur));
                    }
                    break;
                case LOAD:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (name_regi[cur.rd] == 0) {

                    }
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(lui(2, id_addr[cur.rd]), cur));
                    instructions.push_back(std::make_pair(lw(rd, 2, id_addr[cur.rd]), cur));
                    break;
                case ADDI:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    if (cur.rd == "x10") rd = 10;
                    if (ifnum(cur.rs1) && ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(2, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(addi(rd, 2, stoi(cur.rs2)), cur));
                    }
                    else if (ifnum(cur.rs1)) {
                        instructions.push_back(std::make_pair(addi(rd, name_regi[cur.rs2], stoi(cur.rs1)), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    else if (ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(rd, name_regi[cur.rs1], stoi(cur.rs2)), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    else {
                        instructions.push_back(std::make_pair(add(rd, name_regi[cur.rs1], name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs1]] = false;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    if (cur.rd == "x10") {
                        instructions.push_back(std::make_pair(0xff00513, cur));
                        addr += 4;
                    }
                    break;
                case SUBI:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    if (ifnum(cur.rs1) && ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(3, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(addi(4, 0, stoi(cur.rs2)), cur));
                        instructions.push_back(std::make_pair(sub(rd, 3, 4), cur));
                    }
                    else if (ifnum(cur.rs1)) {
                        instructions.push_back(std::make_pair(addi(3, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(sub(rd, 3, name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    else if (ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(4, 0, stoi(cur.rs2)), cur));
                        instructions.push_back(std::make_pair(sub(rd, name_regi[cur.rs1], 4), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    else {
                        instructions.push_back(std::make_pair(sub(rd, name_regi[cur.rs1], name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs1]] = false;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    break;
                case MULI:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    if (ifnum(cur.rs1) && ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(5, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(addi(6, 0, stoi(cur.rs2)), cur));
                        instructions.push_back(std::make_pair(mul(rd, 5, 6), cur));
                    }
                    else if (ifnum(cur.rs1)) {
                        instructions.push_back(std::make_pair(addi(5, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(mul(rd, 5, name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    else if (ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(6, 0, stoi(cur.rs2)), cur));
                        instructions.push_back(std::make_pair(mul(rd, name_regi[cur.rs1], 6), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    else {
                        instructions.push_back(std::make_pair(mul(rd, name_regi[cur.rs1], name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs1]] = false;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    break;
                case DIVI:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    if (ifnum(cur.rs1) && ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(7, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(addi(8, 0, stoi(cur.rs2)), cur));
                        instructions.push_back(std::make_pair(div(rd, 7, 8), cur));
                    }
                    else if (ifnum(cur.rs1)) {
                        instructions.push_back(std::make_pair(addi(7, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(div(rd, 7, name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    else if (ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(8, 0, stoi(cur.rs2)), cur));
                        instructions.push_back(std::make_pair(div(rd, name_regi[cur.rs1], 8), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    else {
                        for (int i = 11; i <= 32; ++i);
                        instructions.push_back(std::make_pair(div(rd, name_regi[cur.rs1], name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs1]] = false;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    break;
                case ANDI:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    if (ifnum(cur.rs1) && ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(2, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(andi(rd, 2, stoi(cur.rs2)), cur));
                    }
                    else if (ifnum(cur.rs1)) {
                        instructions.push_back(std::make_pair(andi(rd, name_regi[cur.rs2], stoi(cur.rs1)), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    else if (ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(andi(rd, name_regi[cur.rs1], stoi(cur.rs2)), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    else {
                        instructions.push_back(std::make_pair(_and(rd, name_regi[cur.rs1], name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs1]] = false;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    break;
                case ORI:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    if (ifnum(cur.rs1) && ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(addi(3, 0, stoi(cur.rs1)), cur));
                        instructions.push_back(std::make_pair(ori(rd, 3, stoi(cur.rs2)), cur));
                    }
                    else if (ifnum(cur.rs1)) {
                        instructions.push_back(std::make_pair(ori(rd, name_regi[cur.rs2], stoi(cur.rs1)), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    else if (ifnum(cur.rs2)) {
                        instructions.push_back(std::make_pair(ori(rd, name_regi[cur.rs1], stoi(cur.rs2)), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    else {
                        instructions.push_back(std::make_pair(_or(rd, name_regi[cur.rs1], name_regi[cur.rs2]), cur));
                        regiused[name_regi[cur.rs1]] = false;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    break;
                case JAL:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    cur.addr = addr;
                    instructions.push_back(std::make_pair(0, cur));
                    addr += 4;
                    break;
                case EQ:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (ifnum(cur.rs1)) instructions.push_back(std::make_pair(addi(3, 0, stoi(cur.rs1)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(3, name_regi[cur.rs1], 0), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    if (ifnum(cur.rs2)) instructions.push_back(std::make_pair(addi(4, 0, stoi(cur.rs2)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(4, name_regi[cur.rs2], 0), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    instructions.push_back(std::make_pair(slt(5, 3, 4), cur));
                    instructions.push_back(std::make_pair(slt(6, 4, 3), cur));
                    instructions.push_back(std::make_pair(_or(7, 5, 6), cur));
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(xori(rd, 7, 1), cur));
                    break;
                case NEQ:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (ifnum(cur.rs1)) instructions.push_back(std::make_pair(addi(5, 0, stoi(cur.rs1)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(5, name_regi[cur.rs1], 0), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    if (ifnum(cur.rs2)) instructions.push_back(std::make_pair(addi(6, 0, stoi(cur.rs2)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(6, name_regi[cur.rs2], 0), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    instructions.push_back(std::make_pair(slt(7, 5, 6), cur));
                    instructions.push_back(std::make_pair(slt(8, 6, 5), cur));
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(_or(rd, 7, 8), cur));
                    break;
                case LT:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (ifnum(cur.rs1)) instructions.push_back(std::make_pair(addi(7, 0, stoi(cur.rs1)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(7, name_regi[cur.rs1], 0), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    if (ifnum(cur.rs2)) instructions.push_back(std::make_pair(addi(8, 0, stoi(cur.rs2)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(8, name_regi[cur.rs2], 0), cur));
                        addr += 4;
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(slt(rd, 7, 8), cur));
                    break;
                case LTE:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (ifnum(cur.rs1)) instructions.push_back(std::make_pair(addi(3, 0, stoi(cur.rs1)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(3, name_regi[cur.rs1], 0), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    if (ifnum(cur.rs2)) instructions.push_back(std::make_pair(addi(4, 0, stoi(cur.rs2)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(4, name_regi[cur.rs2], 0), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    instructions.push_back(std::make_pair(slt(5, 3, 4), cur));
                    instructions.push_back(std::make_pair(slt(6, 4, 3), cur));
                    instructions.push_back(std::make_pair(_or(7, 5, 6), cur));
                    instructions.push_back(std::make_pair(xori(8, 7, 1), cur));
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(_or(rd, 5, 8), cur));
                    break;
                case GR:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (ifnum(cur.rs1)) instructions.push_back(std::make_pair(addi(5, 0, stoi(cur.rs1)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(5, name_regi[cur.rs1], 0), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    if (ifnum(cur.rs2)) instructions.push_back(std::make_pair(addi(6, 0, stoi(cur.rs2)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(6, name_regi[cur.rs2], 0), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(slt(rd, 6, 5), cur));
                    break;
                case GRE:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    if (ifnum(cur.rs1)) instructions.push_back(std::make_pair(addi(7, 0, stoi(cur.rs1)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(7, name_regi[cur.rs1], 0), cur));
                        regiused[name_regi[cur.rs1]] = false;
                    }
                    if (ifnum(cur.rs2)) instructions.push_back(std::make_pair(addi(8, 0, stoi(cur.rs2)), cur));
                    else {
                        instructions.push_back(std::make_pair(addi(8, name_regi[cur.rs2], 0), cur));
                        regiused[name_regi[cur.rs2]] = false;
                    }
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(slt(3, 7, 8), cur));
                    instructions.push_back(std::make_pair(slt(4, 8, 7), cur));
                    instructions.push_back(std::make_pair(_or(5, 3, 4), cur));
                    instructions.push_back(std::make_pair(xori(6, 5, 1), cur));
                    for (int i = 11; i <= 32; ++i)
                        if (regiused[i] == false) {
                            rd = i;
                            name_regi[cur.rd] = i;
                            regiused[i] = true;
                            break;
                        }
                    instructions.push_back(std::make_pair(_or(rd, 4, 6), cur));
                    break;
                case BEQ:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    cur.rsrs1 = name_regi[cur.rs1];
                    cur.rsrs2 = name_regi[cur.rs2];
                    cur.addr = addr;
                    instructions.push_back(std::make_pair(0, cur));
                    addr += 4;
                    break;
                case BNE:
                    if (line_addr.count(cur.line) == 0) line_addr[cur.line] = addr;
                    cur.rsrs1 = name_regi[cur.rs1];
                    cur.rsrs2 = name_regi[cur.rs2];
                    cur.addr = addr;
                    instructions.push_back(std::make_pair(0, cur));
                    addr += 4;
                    break;
            }
        }
        for (int i = 0; i < instructions.size(); ++i) {
            int jumpaddr = line_addr[instructions[i].second.num] - instructions[i].second.addr;
            switch (instructions[i].second.co) {
                case JAL:
                    instructions[i].first = jal(1, jumpaddr);
                    break;
                case BEQ:
                    instructions[i].first = beq(instructions[i].second.rsrs1, instructions[i].second.rsrs2, jumpaddr);
                    break;
                case BNE:
                    instructions[i].first = bne(instructions[i].second.rsrs1, instructions[i].second.rsrs2, jumpaddr);
                    break;
                default:
                    break;
            }
        }
    }





#endif //BASIC_COMPILER_CFG_HPP
