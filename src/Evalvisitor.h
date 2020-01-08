#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"
#include <cstring>
#include "bigInteger.hpp"
#include <map>
#include <stdlib.h>
#include <vector>
#include <iomanip>
#include <stack>
#include <string>
#include <sstream>

class EvalVisitor: public Python3BaseVisitor {

//todo:override all methods of Python3BaseVisitor
    std::map<std::string, antlrcpp::Any> paraments;//当前层变量
    std::map<std::string, antlrcpp::Any> global_variables;//全局变量
    std::stack<std::vector<std::string>> to_get_value;//对形参赋值（a,b）这种传入方式
    std::stack<std::map<std::string,antlrcpp::Any>> level;//变量的栈
    std::stack<std::map<std::string,antlrcpp::Any>> tempmap;//赋形参变量时的栈
    std::stack<std::vector<antlrcpp::Any>> return_value;//函数返回值
    std::map<std::string, Python3Parser::ParametersContext*> funcparameter;
    std::map<std::string, Python3Parser::SuiteContext*> funcsuite;
    int is_global = 0;

    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        //std::cout<<"visitFile_input"<<'\n';
        std::map<std::string,antlrcpp::Any> temp;
        level.push(temp);
        paraments = level.top();
        for (int i = 0;i < ctx->stmt().size();++i)
        {
            antlrcpp::Any ret = visit(ctx->stmt(i));
            if (ret.is<std::string>() && ret.as<std::string>() == "return")
                return 0;
        }
        level.pop();
        return 0;
    }

    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        //std::cout<<"funcdef\n";
        std::string name = ctx->NAME()->toString();
        std::stack<antlrcpp::Any> temp;
        funcparameter[name] = ctx->parameters();
        funcsuite[name] = ctx->suite();
        return 7;
    }

    antlrcpp::Any visitParameters(Python3Parser::ParametersContext *ctx) override {
        if (ctx->typedargslist() != nullptr) return visit(ctx->typedargslist());
        else return 7;
    }

    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
        for (int i = 0;i < ctx->tfpdef().size();++i)
        {
            std::string temppara = visit(ctx->tfpdef(i)).as<std::string>();
            //std::cout<<temppara<<'\n';
            to_get_value.top().push_back(temppara);
            tempmap.top()[temppara] = -1;
        }
        //std::cout<<"typ size "<<to_get_value.top().size()<<'\n';
        if (ctx->ASSIGN(0) != nullptr)
        {
            for (int i = ctx->ASSIGN().size() - 1,j = ctx->tfpdef().size() - 1;i >= 0;--i,--j)
            {
                std::string temppara = visit(ctx->tfpdef(j)).as<std::string>();
                antlrcpp::Any ret = visit(ctx->test(i));
                if (ret.is<std::vector<antlrcpp::Any>>())
                {
                    std::vector<antlrcpp::Any> the_vector = ret.as<std::vector<antlrcpp::Any>>();
                    if (the_vector[0].is<std::vector<antlrcpp::Any>>())
                        ret = the_vector[0];
                }
                tempmap.top()[temppara] = ret;
            }
        }
        return 7;
    }

    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
        std::string str = ctx->NAME()->toString();
        return str;
    }

    antlrcpp::Any visitStmt(Python3Parser::StmtContext *ctx) override {
        //std::cout<<"visitStmt"<<'\n';
        if (ctx->simple_stmt() != nullptr) return visit(ctx->simple_stmt());
        if (ctx->compound_stmt() != nullptr) return visit(ctx->compound_stmt());
        return 0;
    }

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
        //std::cout<<"visitSimple_stmt"<<'\n';
        return visit(ctx->small_stmt());
    }

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
        //std::cout<<"visitSmall_stmt"<<'\n';
        if (ctx->expr_stmt() != nullptr) return visit(ctx->expr_stmt());
        if (ctx->flow_stmt() != nullptr) return visit(ctx->flow_stmt());
        return 0;
    }

    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
        //std::cout<<"visitExpr_stmt"<<'\n';
        if (ctx->testlist().size() == 1){
            antlrcpp::Any ret = visit(ctx->testlist(0));
            if (ret.is<std::string>())
            {
                std::string str = ret.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    paraments[str] = -1;
            }
            return ret;//vector
        }
        if (ctx->augassign() != nullptr)
        {
            antlrcpp::Any any_augassign = visit(ctx->augassign());
            std::string str_augassign = any_augassign.as<std::string>();
            antlrcpp::Any ret1,ret2;
            ret1 = visit(ctx->testlist(0));
            ret1 = ret1.as<std::vector<antlrcpp::Any>>()[0];
            std::string ret1str;
            if (ret1.is<std::vector<antlrcpp::Any>>())
                ret1 = ret1.as<std::vector<antlrcpp::Any>>()[0];
            if (ret1.is<std::string>()) {
                ret1str = ret1.as<std::string>();
                if (ret1str[0] != '\'' && ret1str[0] != '\"')
                    ret1 = paraments[ret1str];
            }
            if (ret1.is<int>())
            {
                int p = ret1.as<int>();
                bigInteger tp;
                if (p == 1) tp = "1";
                if (p == 0) tp = "0";
                ret1 = tp;
            }
            ret2 = visit(ctx->testlist(1));
            ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
            std::string ret2str;
            if (ret2.is<std::string>()) {
                ret2str = ret2.as<std::string>();
                if (ret2str[0] != '\'' && ret2str[0] != '\"')
                {
                    ret2 = paraments[ret2str];
                    if (ret2.is<std::string>()) ret2str = ret2.as<std::string>();
                }
            }
            if (ret2.is<int>())
            {
                int p = ret2.as<int>();
                bigInteger tp;
                if (p == 1) tp = "1";
                if (p == 0) tp = "0";
                ret2 = tp;
            }
            if (str_augassign == "+="){
                if (ret1.is<std::string>() && ret2.is<std::string>())
                {
                    std::string temp;
                    temp = ret1.as<std::string>();
                    temp.erase(temp.size() - 1,1);
                    ret2str.erase(0,1);
                    temp += ret2str;
                    paraments[ret1str] = temp;
                }
                if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
                {
                    bigInteger ret1bigint, ret2bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1bigint += ret2bigint;
                    paraments[ret1str] = ret1bigint;
                }
                if (ret1.is<double>() && ret2.is<double>())
                {
                    double ret1double, ret2double;
                    ret1double = ret1.as<double>();
                    ret2double = ret2.as<double>();
                    ret1double += ret2double;
                    paraments[ret1str] = ret1double;
                }
                if (ret1.is<double>() && ret2.is<bigInteger>())
                {
                    double ret1double;
                    bigInteger ret2bigint;
                    ret1double = ret1.as<double>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1double += (double)ret2bigint;
                    paraments[ret1str] = ret1double;
                }
                if (ret2.is<double>() && ret1.is<bigInteger>())
                {
                    double ret2double;
                    bigInteger ret1bigint;
                    ret2double = ret2.as<double>();
                    ret1bigint = ret1.as<bigInteger>();
                    ret2double += (double)ret1bigint;
                    paraments[ret1str] = ret2double;
                }
            }
            if (str_augassign == "-="){
                if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
                {
                    bigInteger ret1bigint, ret2bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1bigint -= ret2bigint;
                    paraments[ret1str] = ret1bigint;
                }
                if (ret1.is<double>() && ret2.is<double>())
                {
                    double ret1double, ret2double;
                    ret1double = ret1.as<double>();
                    ret2double = ret2.as<double>();
                    ret1double -= ret2double;
                    paraments[ret1str] = ret1double;
                }
                if (ret1.is<double>() && ret2.is<bigInteger>())
                {
                    double ret1double;
                    bigInteger ret2bigint;
                    ret1double = ret1.as<double>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1double -= (double)ret2bigint;
                    paraments[ret1str] = ret1double;
                }
                if (ret2.is<double>() && ret1.is<bigInteger>())
                {
                    double ret2double;
                    bigInteger ret1bigint;
                    ret2double = ret2.as<double>();
                    ret1bigint = ret1.as<bigInteger>();
                    ret2double -= (double)ret1bigint;
                    paraments[ret1str] = -ret2double;
                }
            }
            if (str_augassign == "*="){
                if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
                {
                    bigInteger ret1bigint, ret2bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1bigint *= ret2bigint;
                    paraments[ret1str] = ret1bigint;
                }
                if (ret1.is<double>() && ret2.is<double>())
                {
                    double ret1double, ret2double;
                    ret1double = ret1.as<double>();
                    ret2double = ret2.as<double>();
                    ret1double *= ret2double;
                    paraments[ret1str] = ret1double;
                }
                if (ret1.is<double>() && ret2.is<bigInteger>())
                {
                    double ret1double;
                    bigInteger ret2bigint;
                    ret1double = ret1.as<double>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1double *= (double)ret2bigint;
                    paraments[ret1str] = ret1double;
                }
                if (ret2.is<double>() && ret1.is<bigInteger>())
                {
                    double ret2double;
                    bigInteger ret1bigint;
                    ret2double = ret2.as<double>();
                    ret1bigint = ret1.as<bigInteger>();
                    ret2double *= (double)ret1bigint;
                    paraments[ret1str] = ret2double;
                }
                bigInteger zero("0");
                if (ret1.is<bigInteger>() && ret2.is<std::string>())
                {
                    bigInteger ret1bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    bigInteger i("1"),num1("1");
                    std::string basestring = ret2str;
                    if (ret1bigint > num1) ret2str.erase(ret2str.size() - 1,1);
                    basestring.erase(0,1);
                    if (ret1bigint <= zero) ret2str = "\'\'";
                    for ( ;i < ret1bigint;i += num1) {
                        std::string temp = basestring;
                        if (i != ret1bigint - num1) temp.erase(temp.length() - 1,1);
                        ret2str += temp;
                    }
                    paraments[ret1str] = ret2str;
                }
                if (ret2.is<bigInteger>() && ret1.is<std::string>())
                {
                    bigInteger ret2bigint;
                    ret2bigint = ret2.as<bigInteger>();
                    bigInteger i("1"),num1("1");
                    std::string basestring = ret1.as<std::string>();
                    std::string ansstr = basestring;
                    if (ret2bigint > num1) ansstr.erase(ansstr.size() - 1,1);
                    basestring.erase(0,1);
                    if (ret2bigint <= zero) ansstr = "\'\'";
                    for ( ;i < ret2bigint;i += num1) {
                        std::string temp = basestring;
                        if (i != ret2bigint - num1) temp.erase(temp.length() - 1,1);
                        ansstr += temp;
                    }
                    paraments[ret1str] = ansstr;
                }
            }
            if (str_augassign == "/="){
                if (ret1.is<double>() && ret2.is<double>())
                {
                    double ret1double, ret2double;
                    ret1double = ret1.as<double>();
                    ret2double = ret2.as<double>();
                    ret1double /= ret2double;
                    paraments[ret1str] = ret1double;
                }
                if (ret1.is<double>() && ret2.is<bigInteger>())
                {
                    double ret1double;
                    bigInteger ret2bigint;
                    ret1double = ret1.as<double>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1double /= (double)ret2bigint;
                    paraments[ret1str] = ret1double;
                }
                if (ret2.is<double>() && ret1.is<bigInteger>())
                {
                    double ret2double,ans;
                    bigInteger ret1bigint;
                    ret2double = ret2.as<double>();
                    ret1bigint = ret1.as<bigInteger>();
                    ans = (double)ret1bigint / ret2double;
                    paraments[ret1str] = ans;
                }
                if (ret2.is<bigInteger>() && ret1.is<bigInteger>())
                {
                    double ans;
                    bigInteger ret1bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    ans = (double)ret1bigint / (double)ret2.as<bigInteger>();
                    paraments[ret1str] = ans;
                }
            }
            if (str_augassign == "//="){
                if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
                {
                    bigInteger ret1bigint, ret2bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1bigint /= ret2bigint;
                    paraments[ret1str] = ret1bigint;
                }
            }
            if (str_augassign == "%="){
                if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
                {
                    bigInteger ret1bigint, ret2bigint;
                    ret1bigint = ret1.as<bigInteger>();
                    ret2bigint = ret2.as<bigInteger>();
                    ret1bigint = ret1bigint % ret2bigint;
                    paraments[ret1str] = ret1bigint;
                }
            }
        }
        if (ctx->ASSIGN(0) != nullptr)
        {
            antlrcpp::Any ret1,ret2;
            for (int i = ctx->ASSIGN().size();i > 0;--i)
            {
                ret1 = visit(ctx->testlist(i - 1));
                ret2 = visit(ctx->testlist(i));
                std::vector<antlrcpp::Any> para,contain;
                para = ret1.as<std::vector<antlrcpp::Any>>();
                contain = ret2.as<std::vector<antlrcpp::Any>>();
                if (contain[0].is<std::vector<antlrcpp::Any>>())
                    contain = contain[0].as<std::vector<antlrcpp::Any>>();
                std::map<std::string,antlrcpp::Any> assigntemp = paraments;
                for (int j = 0;j < para.size();++j)
                {
                    std::string str = para[j].as<std::string>();
                    std::string comparestr;
                    if (contain[j].is<std::string>())
                    {
                        comparestr = contain[j].as<std::string>();
                        if (comparestr[0] != '\'' && comparestr[0] != '\"')
                            contain[j] = assigntemp[comparestr];
                    }
                        paraments[str] = contain[j];
                    if (contain[j].is<std::vector<antlrcpp::Any>>())
                    {
                        std::vector<antlrcpp::Any> in = contain[j].as<std::vector<antlrcpp::Any>>();
                        for (int ji = 0;ji < in.size();++ji,++j)
                        {
                            str = para[j].as<std::string>();
                            if (in[ji].is<std::string>())
                            {
                                comparestr = in[ji].as<std::string>();
                                if (comparestr[0] != '\'' && comparestr[0] != '\"')
                                    in[ji] = assigntemp[comparestr];
                            }
                            paraments[str] = in[ji];
                        }
                    }
                }
            }
        }
        return 7;
    }

    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *ctx) override {
        std::string str_augassign;
        if (ctx->ADD_ASSIGN()) str_augassign = "+=";
        if (ctx->SUB_ASSIGN()) str_augassign = "-=";
        if (ctx->MULT_ASSIGN()) str_augassign = "*=";
        if (ctx->DIV_ASSIGN()) str_augassign = "/=";
        if (ctx->IDIV_ASSIGN()) str_augassign = "//=";
        if (ctx->MOD_ASSIGN()) str_augassign = "%=";
        return str_augassign;
    }

    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
        if (ctx->break_stmt() != nullptr) return visit(ctx->break_stmt());
        if (ctx->continue_stmt() != nullptr) return visit(ctx->continue_stmt());
        if (ctx->return_stmt() != nullptr) return visit(ctx->return_stmt());
    }

    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
        std::string str = "break";
        return str;
    }

    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        std::string str = "continue";
        return str;
    }

    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
        std::string str = "return";
        if (ctx->testlist() != nullptr){
            antlrcpp::Any ret = visit(ctx->testlist());
            std::vector<antlrcpp::Any> temp;
            if (ret.is<std::vector<antlrcpp::Any>>()) {
                //std::cout<<"vector\n";
                temp = ret.as<std::vector<antlrcpp::Any>>();
                std::vector<antlrcpp::Any> add_to_temp;
                for (int i = 0;i < temp.size();++i)
                {
                    if (!temp[i].is<std::vector<antlrcpp::Any>>()) {
                        if (temp[i].is<std::string>())
                        {
                            //std::cout<<"pushstr\n";
                            std::string str = temp[i].as<std::string>();
                            if (str[0] != '\'' && str[0] != '\"')
                                temp[i] = paraments[str];
                        }
                        add_to_temp.push_back(temp[i]);
                    }
                    else{
                        //std::cout<<"in_vector\n";
                        std::vector<antlrcpp::Any> ttemp = temp[i].as<std::vector<antlrcpp::Any>>();
                        for (int j = 0;j < ttemp.size();++j)
                        {
                            if (ttemp[j].is<std::string>())
                            {
                                std::string str = ttemp[j].as<std::string>();
                                if (str[0] != '\'' && str[0] != '\"')
                                    ttemp[j] = paraments[str];
                            }
                            add_to_temp.push_back(ttemp[j]);
                        }
                    }
                }
                ret = add_to_temp;
            }
            else
            {
                if (ret.is<std::string>())
                {
                    std::string str1 = ret.as<std::string>();
                    if (str1[0] != '\'' && str1[0] != '\"')
                        ret = paraments[str1];
                }
                if(!(ret.is<int>() && ret.as<int>() == 7)) temp.push_back(ret);
                ret = temp;
            }
            return_value.top() = ret.as<std::vector<antlrcpp::Any>>();
        }
        return str;
    }

    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override {
        if (ctx->if_stmt() != nullptr) return visit(ctx->if_stmt());
        if (ctx->while_stmt() != nullptr) return visit(ctx->while_stmt());
        if (ctx->funcdef() != nullptr) return visit(ctx->funcdef());
        return 7;
    }

    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        for (int i = 0;i < ctx->test().size();++i)
        {
            antlrcpp::Any ret1 = visit(ctx->test(i));
            if (ret1.is<std::vector<antlrcpp::Any>>())
                ret1 = ret1.as<std::vector<antlrcpp::Any>>()[0];
            if (ret1.is<std::string>())
            {
                std::string str = ret1.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    ret1 = paraments[str];
            }
            if (ret1.is<bigInteger>())
            {
                ret1 = (bool)ret1.as<bigInteger>();
                ret1 = (int)ret1.as<bool>();
            }
            if (ret1.is<double>())
            {
                ret1 = (bool)ret1.as<double>();
                ret1 = (int)ret1.as<bool>();
            }
            if (ret1.is<std::string>())
            {
                std::string strret1 = ret1.as<std::string>();
                strret1.erase(0,1);
                strret1.erase(strret1.size()-1,1);
                if (strret1 == "") ret1 = int(0);
                else ret1 = int(1);
            }
            int check = ret1.as<int>();
            if (check == 1)
            {
                antlrcpp::Any retyes = visit(ctx->suite(i));
                if (retyes.is<std::string>())
                {
                    std::string to_do = retyes.as<std::string>();
                    if (to_do == "break" || to_do == "continue" || to_do == "return")
                        return to_do;
                }
                return 7;
            }
            else continue;
        }
        if (ctx->ELSE() != nullptr) return visit(ctx->suite(ctx->suite().size() - 1));
        return 7;
    }

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        while (true)
        {
            antlrcpp::Any ret1 = visit(ctx->test());
            if (ret1.is<std::vector<antlrcpp::Any>>())
                ret1 = ret1.as<std::vector<antlrcpp::Any>>()[0];
            if (ret1.is<std::string>())
            {
                std::string str = ret1.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    ret1 = paraments[str];
            }
            if (ret1.is<bigInteger>())
            {
                ret1 = (bool)ret1.as<bigInteger>();
                ret1 = (int)ret1.as<bool>();
            }
            if (ret1.is<double>())
            {
                ret1 = (bool)ret1.as<double>();
                ret1 = (int)ret1.as<bool>();
            }
            if (ret1.is<std::string>())
            {
                std::string strret1 = ret1.as<std::string>();
                strret1.erase(0,1);
                strret1.erase(strret1.size()-1,1);
                if (strret1 == "") ret1 = int(0);
                else ret1 = int(1);
            }
            int check = ret1.as<int>();
            if (check == 0) break;
            antlrcpp::Any temp = visit(ctx->suite());
            if (temp.is<std::string>())
            {
                std::string str;
                str = temp.as<std::string>();
                if (str == "break") break;
                if (str == "continue") continue;
                if (str == "return") return str;
            }
        }
        return 7;
    }

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *ctx) override {
        //std::cout<<"visitsuite\n";
        if (ctx->simple_stmt() != nullptr) return visit(ctx->simple_stmt());
        if (ctx->stmt(0) != nullptr)  {
            for (int i = 0;i < ctx->stmt().size();++i)
            {
                antlrcpp::Any ret = visit(ctx->stmt(i));
                if (ret.is<std::string>())
                {
                    std::string to_do = ret.as<std::string>();
                    if (to_do == "break") return std::string("break");
                    if (to_do == "continue") return std::string("continue");
                    if (to_do == "return") return std::string("return");
                }
            }
        }
        return 7;
    }

    antlrcpp::Any visitTest(Python3Parser::TestContext *ctx) override {
        //std::cout<<"visitTest"<<'\n';
        return visit(ctx->or_test());
    }

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *ctx) override {
        //std::cout<<"or_test"<<'\n';
        if (ctx->and_test().size() == 1) return visit(ctx->and_test(0));
        for(int i = 0;i < ctx->and_test().size();++i)
        {
            antlrcpp::Any ret = visit(ctx->and_test(i));
            if (ret.is<std::vector<antlrcpp::Any>>())
                ret = ret.as<std::vector<antlrcpp::Any>>()[0];
            if (ret.is<std::string>())
            {
                std::string str = ret.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    ret = paraments[str];
            }
            if (ret.is<bigInteger>())
            {
                ret = (bool)ret.as<bigInteger>();
                ret = (int)ret.as<bool>();
            }
            if (ret.is<double>())
            {
                ret = (bool)ret.as<double>();
                ret = (int)ret.as<bool>();
            }
            if (ret.is<std::string>())
            {
                std::string strret1 = ret.as<std::string>();
                strret1.erase(0,1);
                strret1.erase(strret1.size()-1,1);
                if (strret1 == "") ret = int(0);
                else ret = int(1);
            }
            int es = ret.as<int>();
            if (es == 1) return 1;
        }
        return 0;
    }

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *ctx) override {
        //std::cout<<"and_test"<<'\n';
        if (ctx->not_test().size() == 1) return visit(ctx->not_test(0));
        for (int i = 0;i < ctx->not_test().size();++i)
        {
            antlrcpp::Any ret = visit(ctx->not_test(i));
            if (ret.is<std::vector<antlrcpp::Any>>())
                ret = ret.as<std::vector<antlrcpp::Any>>()[0];
            if (ret.is<std::string>())
            {
                std::string str = ret.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    ret = paraments[str];
            }
            if (ret.is<bigInteger>())
            {
                ret = (bool)ret.as<bigInteger>();
                ret = (int)ret.as<bool>();
            }
            if (ret.is<double>())
            {
                ret = (bool)ret.as<double>();
                ret = (int)ret.as<bool>();
            }
            if (ret.is<std::string>())
            {
                std::string strret1 = ret.as<std::string>();
                strret1.erase(0,1);
                strret1.erase(strret1.size()-1,1);
                if (strret1 == "") ret = int(0);
                else ret = int(1);
            }
            int es = ret.as<int>();
            if (es == 0) return 0;
        }
        return 1;
    }

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *ctx) override {
        //std::cout<<"not_test"<<'\n';
        if (ctx->not_test() != nullptr) {
            antlrcpp::Any ret = visit(ctx->not_test());
            if (ret.is<std::vector<antlrcpp::Any>>())
                ret = ret.as<std::vector<antlrcpp::Any>>()[0];
            if (ret.is<std::string>())
            {
                std::string str = ret.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    ret = paraments[str];
            }
            if (ret.is<bigInteger>())
            {
                ret = (bool)ret.as<bigInteger>();
                ret = (int)ret.as<bool>();
            }
            if (ret.is<double>())
            {
                ret = (bool)ret.as<double>();
                ret = (int)ret.as<bool>();
            }
            if (ret.is<std::string>())
            {
                std::string strret1 = ret.as<std::string>();
                strret1.erase(0,1);
                strret1.erase(strret1.size()-1,1);
                if (strret1 == "") ret = int(0);
                else ret = int(1);
            }
            int es = ret.as<int>();
            if (es == 1) return 0;
            if (es == 0) return 1;
        }
        if (ctx->comparison() != nullptr) return visit(ctx->comparison());
        return 0;
    }

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *ctx) override {
        //std::cout<<"comparison"<<'\n';
        if (ctx->arith_expr().size() == 1) return visit(ctx->arith_expr(0));
        antlrcpp::Any ret1 = visit(ctx->arith_expr(0));
        std::string ret1str;
        if (ret1.is<std::vector<antlrcpp::Any>>())
            ret1 = ret1.as<std::vector<antlrcpp::Any>>()[0];
        if(ret1.is<std::string>()) {
            ret1str = ret1.as<std::string>();
            if (ret1str[0] != '\'' && ret1str[0] != '\"') {
                ret1 = paraments[ret1str];
            }
        }
        int n = ctx->arith_expr().size();
        for (int i = 1;i < n;++i)
        {
            antlrcpp::Any compOp = visit(ctx->comp_op(i - 1));
            std::string compstr = compOp.as<std::string>();
            antlrcpp::Any ret2 = visit(ctx->arith_expr(i));
            std::string ret2str;
            if (ret2.is<std::vector<antlrcpp::Any>>())
                ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
            if(ret2.is<std::string>()) {
                ret2str = ret2.as<std::string>();
                if (ret2str[0] != '\'' && ret2str[0] != '\"')
                    ret2 = paraments[ret2str];
            }
            if (ret1.is<int>() && ret2.is<int>())
            {
                int int1 = ret1.as<int>(), int2 = ret2.as<int>();
                if (compstr == "<")
                    if (int1 >= int2) return 0;
                if (compstr == ">")
                    if (int1 <= int2) return 0;
                if (compstr == "==")
                    if (int1 != int2) return 0;
                if (compstr == ">=")
                    if (int1 < int2) return 0;
                if (compstr == "<=")
                    if (int1 > int2) return 0;
                if (compstr == "!=")
                    if (int1 == int2) return 0;
            }
            bigInteger one("1"),zero("0");
            if (ret1.is<int>() && !ret2.is<int>())
            {
                int theint = ret1.as<int>();
                if (theint == 1) ret1 = one;
                if (theint == 0) ret1 = zero;
            }
            if (!ret1.is<int>() && ret2.is<int>())
            {
                int theint = ret2.as<int>();
                if (theint == 1) ret2 = one;
                if (theint == 0) ret2 = zero;
            }
            if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
            {
                bigInteger bigint1 = ret1.as<bigInteger>(), bigint2 = ret2.as<bigInteger>();
                if (compstr == "<")
                    if (!(bigint1 < bigint2)) return 0;
                if (compstr == ">")
                    if (!(bigint1 > bigint2)) return 0;
                if (compstr == "==")
                    if (!(bigint1 == bigint2)) return 0;
                if (compstr == ">=")
                    if (!(bigint1 >= bigint2)) return 0;
                if (compstr == "<=")
                    if (!(bigint1 <= bigint2)) return 0;
                if (compstr == "!=")
                    if (!(bigint1 != bigint2)) return 0;
            }
            if (ret1.is<std::string>() && ret2.is<std::string>())
            {
                std::string str1 = ret1.as<std::string>();
                std::string str2 = ret2.as<std::string>();
                char *pstr1 = &str1[0], *pstr2 = &str2[0];
                int result = strcmp(pstr1,pstr2);
                if (compstr == "<")
                    if (result >= 0) return 0;
                if (compstr == ">")
                    if (result <= 0) return 0;
                if (compstr == "==")
                    if (result != 0) return 0;
                if (compstr == ">=")
                    if (result < 0) return 0;
                if (compstr == "<=")
                    if (result > 0) return 0;
                if (compstr == "!=")
                    if (result == 0) return 0;
            }
            if (ret1.is<double>() || ret2.is<double>())
            {
                if (ret1.is<bigInteger>()) ret1 = (double)ret1.as<bigInteger>();
                if (ret2.is<bigInteger>()) ret2 = (double)ret2.as<bigInteger>();
                double double1 = ret1.as<double>(), double2 = ret2.as<double>();
                if (compstr == "<")
                    if (double1 >= double2) return 0;
                if (compstr == ">")
                    if (double1 <= double2) return 0;
                if (compstr == "==")
                    if (double1 != double2) return 0;
                if (compstr == ">=")
                    if (double1 < double2) return 0;
                if (compstr == "<=")
                    if (double1 > double2) return 0;
                if (compstr == "!=")
                    if (double1 == double2) return 0;
            }
            ret1 = ret2;
        }
        return 1;
    }//加

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *ctx) override {
        std::string str;
        if (ctx->LESS_THAN()) str = "<";
        if (ctx->GREATER_THAN()) str = ">";
        if (ctx->EQUALS()) str = "==";
        if (ctx->GT_EQ()) str = ">=";
        if (ctx->LT_EQ()) str = "<=";
        if (ctx->NOT_EQ_2()) str = "!=";
        return str;
    }

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
        //std::cout<<"arith_expr"<<'\n';
        if (ctx->term().size() == 1) return visit(ctx->term(0));
        antlrcpp::Any type;
        type = visit(ctx->term(0));
        if (type.is<std::vector<antlrcpp::Any>>())
            type = type.as<std::vector<antlrcpp::Any>>()[0];
        if (type.is<std::string>())
        {
            std::string typestr = type.as<std::string>();
           if (typestr[0] != '\'' && typestr[0] != '\"') type = paraments[typestr];
        }
        if (type.is<int>())
        {
            bigInteger exc;
            if (type.as<int>() == 1) exc = "1";
            if (type.as<int>() == 0) exc = "0";
            type = exc;
        }
        if (type.is<std::string>())
        {
            std::string str;
            str = type.as<std::string>();
            str.erase(str.size() - 1,1);
            for (int i = 1;i < ctx->term().size();++i)
            {
                antlrcpp::Any temp = visit(ctx->term(i));
                if (temp.is<std::vector<antlrcpp::Any>>())
                    temp = temp.as<std::vector<antlrcpp::Any>>()[0];
                std::string tempstr;
                tempstr = temp.as<std::string>();
                if (tempstr[0] != '\'' && tempstr[0] != '\"')
                {
                    temp = paraments[tempstr];
                    if (temp.is<std::string>()) tempstr = temp.as<std::string>();
                }
                if (i != ctx->term().size() - 1) tempstr.erase(tempstr.size() - 1,1);
                tempstr.erase(0,1);
                str += tempstr;
            }
            return str;
        }
        if (type.is<bigInteger>())
        {
            for (int i = 0;i < ctx->term().size();++i)
            {
                antlrcpp::Any check = visit(ctx->term(i));
                if (check.is<std::vector<antlrcpp::Any>>())
                    check = check.as<std::vector<antlrcpp::Any>>()[0];
                if (check.is<std::string>())
                {
                    std::string checkstr = check.as<std::string>();
                    check = paraments[checkstr];
                }
                if (check.is<double>())
                {
                    type = (double)type.as<bigInteger>();
                    break;
                }
            }
        }
        if (type.is<bigInteger>())
        {
            bigInteger ans;
            ans = type.as<bigInteger>();
            for (int i = 1, ia = 0, im = 0;i < ctx->term().size();++i)
            {
                antlrcpp::Any temp = visit(ctx->term(i));
                if (temp.is<std::vector<antlrcpp::Any>>())
                    temp = temp.as<std::vector<antlrcpp::Any>>()[0];
                std::string tempstr;
                if(temp.is<std::string>()) {
                    tempstr = temp.as<std::string>();
                    if (tempstr[0] != '\'' && tempstr[0] != '\"') temp = paraments[tempstr];
                }
                if (temp.is<int>())
                {
                    bigInteger extp;
                    if (temp.as<int>() == 1) extp = "1";
                    if (temp.as<int>() == 0) extp = "0";
                    temp = extp;
                }
                if (ctx->ADD().size() <= ia)
                {
                    ans -= temp.as<bigInteger>();
                }
                else if (ctx->MINUS().size() <= im)
                {
                    ans += temp.as<bigInteger>();
                }
                else if (ctx->ADD(ia)->getSymbol()->getTokenIndex() < ctx->MINUS(im)->getSymbol()->getTokenIndex())
                {
                    ans += temp.as<bigInteger>();
                    ++ia;
                }
                else
                {
                    ans -= temp.as<bigInteger>();
                    ++im;
                }
            }
            return ans;
        }
        if (type.is<double>())
        {
            double ans;
            ans = type.as<double>();
            for (int i = 1, ia = 0, im = 0;i < ctx->term().size();++i)
            {
                antlrcpp::Any temp = visit(ctx->term(i));
                if (temp.is<std::vector<antlrcpp::Any>>())
                    temp = temp.as<std::vector<antlrcpp::Any>>()[0];
                std::string tempstr;
                if(temp.is<std::string>()) {
                    tempstr = temp.as<std::string>();
                    if (tempstr[0] != '\'' && tempstr[0] != '\"')
                        temp = paraments[tempstr];
                }
                if (temp.is<int>()) temp = (double)temp.as<int>();
                if (temp.is<bigInteger>()) temp = (double)temp.as<bigInteger>();
                if (ctx->ADD().size() <= ia)
                {
                    ans -= temp.as<double>();
                }
                else if (ctx->MINUS().size() <= im)
                {
                    ans += temp.as<double>();
                }
                else if (ctx->ADD(ia)->getSymbol()->getTokenIndex() < ctx->MINUS(im)->getSymbol()->getTokenIndex())
                {
                    ans += temp.as<double>();
                    ++ia;
                }
                else
                {
                    ans -= temp.as<double>();
                    ++im;
                }
            }
            return ans;
        }
    }

    antlrcpp::Any visitTerm(Python3Parser::TermContext *ctx) override {
        //std::cout<<"term"<<'\n';
        if(ctx->factor().size() == 1) return visit(ctx->factor(0));
        antlrcpp::Any ret1 = visit(ctx->factor(0));
        if (ret1.is<std::vector<antlrcpp::Any>>())
            ret1 = ret1.as<std::vector<antlrcpp::Any>>()[0];
        bigInteger bigans;
        std::string strans;
        if (ret1.is<bigInteger>()) bigans = ret1.as<bigInteger>();
        if (ret1.is<int>()){
            if (ret1.as<int>() == 1) bigans = "1";
            if (ret1.as<int>() == 0) bigans = "0";
            ret1 = bigans;
        }
        if (ret1.is<std::string>())
        {
            strans = ret1.as<std::string>();
            if (strans[0] != '\'' && strans[0] != '\"')
            {
                ret1 = paraments[strans];
                if (ret1.is<bigInteger>()) bigans = ret1.as<bigInteger>();
                if (ret1.is<std::string>()) strans = ret1.as<std::string>();
                if (ret1.is<int>()){
                    if (ret1.as<int>() == 1) bigans = "1";
                    if (ret1.as<int>() == 0) bigans = "0";
                    ret1 = bigans;
                }
            }
        }
        for (int i = 1,istar=0,idiv=0,iidiv=0,imod=0;i < ctx->factor().size();++i)
        {
            antlrcpp::Any ret2 = visit(ctx->factor(i));
            if (ret2.is<std::vector<antlrcpp::Any>>())
                ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
            if (ret2.is<std::string>())
            {
                std::string ret2str = ret2.as<std::string>();
                if (ret2str[0] !='\'' && ret2str[0] != '\"')
                {
                    ret2 = paraments[ret2str];
                }
            }
            if (ret2.is<int>())
            {
                bigInteger ex2;
                if (ret2.as<int>() == 1) ex2 = "1";
                if (ret2.as<int>() == 0) ex2 = "0";
                ret2 = ex2;
            }
            std::string oprt;
            if (ctx->STAR(istar) != nullptr)  oprt = "*";
            if (ctx->DIV(idiv) != nullptr)
            {
                if(oprt == "*") {
                    if (ctx->DIV(idiv)->getSymbol()->getTokenIndex() < ctx->STAR(istar)->getSymbol()->getTokenIndex())
                        oprt = "/";
                }
                else oprt = "/";
            }
            if (ctx->IDIV(iidiv) != nullptr)
            {
                if(oprt == "*"){
                    if (ctx->IDIV(iidiv)->getSymbol()->getTokenIndex() < ctx->STAR(istar)->getSymbol()->getTokenIndex())
                        oprt = "//";
                }
                else if(oprt == "/"){
                    if (ctx->IDIV(iidiv)->getSymbol()->getTokenIndex() < ctx->DIV(idiv)->getSymbol()->getTokenIndex())
                        oprt = "//";
                }
                else oprt = "//";
            }
            if (ctx->MOD(imod) != nullptr)
            {
                if(oprt == "*"){
                    if (ctx->MOD(imod)->getSymbol()->getTokenIndex() < ctx->STAR(istar)->getSymbol()->getTokenIndex())
                        oprt = "%";
                }
                else if(oprt == "/"){
                    if (ctx->MOD(imod)->getSymbol()->getTokenIndex() < ctx->DIV(idiv)->getSymbol()->getTokenIndex())
                        oprt = "%";
                }
                else if(oprt == "//"){
                    if (ctx->MOD(imod)->getSymbol()->getTokenIndex() < ctx->IDIV(iidiv)->getSymbol()->getTokenIndex())
                        oprt = "%";
                }
                else oprt = "%";
            }
            if (oprt == "*"){
                bigInteger zero("0");
                if (ret2.is<bigInteger>() && ret1.is<bigInteger>())
                {
                    bigans *= ret2.as<bigInteger>();
                    ret1 = bigans;
                }
                if (ret1.is<std::string>() && ret2.is<bigInteger>())
                {
                    bigInteger bi("1");
                    bigInteger num_1("1");
                    std::string basestr = strans;
                    strans.erase(strans.size() - 1,1);
                    basestr.erase(0,1);
                    basestr.erase(basestr.length() - 1,1);
                    if (ret2.as<bigInteger>() <= zero) strans = "\"";
                    for ( ;bi < ret2.as<bigInteger>();bi += num_1)
                    {
                        strans += basestr;
                    }
                    strans += strans[0];
                    ret1 = strans;
                }
                if (ret2.is<std::string>() && ret1.is<bigInteger>())
                {
                    bigInteger bi("1");
                    bigInteger num_1("1");
                    std::string basestr = ret2.as<std::string>();
                    strans = basestr;
                    strans.erase(strans.size() - 1,1);
                    if (ret1.as<bigInteger>() <= zero) strans = "\"";
                    basestr.erase(0,1);
                    basestr.erase(basestr.length() - 1,1);
                    for ( ;bi < ret1.as<bigInteger>();bi += num_1)
                    {
                        strans += basestr;
                    }
                    strans += strans[0];
                    ret1 = strans;
                }
                if (ret1.is<double>() && ret2.is<double>())
                {
                    double doubleans;
                    doubleans = ret2.as<double>();
                    doubleans *= ret1.as<double>();
                    ret1 = doubleans;
                }
                if (ret2.is<double>() && ret1.is<bigInteger>())
                {
                    double doubleans;
                    doubleans = (double)ret1.as<bigInteger>();
                    doubleans *= ret2.as<double>();
                    ret1 = doubleans;
                }
                if (ret1.is<double>() && ret2.is<bigInteger>())
                {
                    double doubleans;
                    doubleans = (double)ret2.as<bigInteger>();
                    doubleans *= ret1.as<double>();
                    ret1 = doubleans;
                }
                ++istar;
            }
            if (oprt == "/"){
                if (ret1.is<double>() && ret2.is<double>())
                {
                    double doubleans;
                    doubleans = ret1.as<double>();
                    doubleans /= ret2.as<double>();
                    ret1 = doubleans;
                    ++idiv;
                }
                if (ret1.is<double>() && ret2.is<bigInteger>())
                {
                    double doubleans;
                    doubleans = ret1.as<double>();
                    doubleans /= (double)ret2.as<bigInteger>();
                    ret1 = doubleans;
                    ++idiv;
                }
                if (ret1.is<bigInteger>() && ret2.is<double>())
                {
                    double doubleans;
                    doubleans = (double) ret1.as<bigInteger>();
                    doubleans /= ret2.as<double>();
                    ret1 = doubleans;
                    ++idiv;
                }
                if (ret1.is<bigInteger>() && ret2.is<bigInteger>())
                {
                    double doubleans;
                    doubleans = (double) ret1.as<bigInteger>();
                    doubleans /= (double)ret2.as<bigInteger>();
                    ret1 = doubleans;
                    ++idiv;
                }
            }
            if (oprt == "//"){
                if (ret2.is<bigInteger>() && ret1.is<bigInteger>())
                {
                    bigans /= ret2.as<bigInteger>();
                    ret1 = bigans;
                    ++iidiv;
                }
            }
            if (oprt == "%"){
                if (ret2.is<bigInteger>() && ret1.is<bigInteger>())
                {
                    bigans = bigans % ret2.as<bigInteger>();
                    ret1 = bigans;
                    ++imod;
                }
            }
        }
        return ret1;
    }///!!!!

    antlrcpp::Any visitFactor(Python3Parser::FactorContext *ctx) override {
        //std::cout<<"factor"<<'\n';
        if (ctx->factor() != nullptr) {
            antlrcpp::Any ret = visit(ctx->factor());
            if (ret.is<std::vector<antlrcpp::Any>>())
                ret = ret.as<std::vector<antlrcpp::Any>>()[0];
            if (ret.is<std::string>())
            {
                std::string str = ret.as<std::string>();
                if (str[0] != '\'' && str[0] != '\"')
                    ret = paraments[str];
            }
            if (ctx->MINUS() != nullptr)
            {
                if (ret.is<int>())
                {
                    bigInteger ex;
                    if (ret.as<int>() == 1) ex = "1";
                    if (ret.as<int>() == 0) ex = "0";
                    ret = ex;
                }
                if (ret.is<bigInteger>())
                {
                    bigInteger zero = "0";
                    bigInteger ans = zero - ret.as<bigInteger>();
                    return ans;
                }
                if (ret.is<double>())
                    return -ret.as<double>();
            }
            return ret;
        }
        if (ctx->atom_expr() != nullptr) return visit(ctx->atom_expr());
        return 0;
    }

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        if (ctx->trailer() != nullptr)
        {
            antlrcpp::Any ret1;
            ret1 = visit(ctx->atom());
            std::string cpstr = "\t";
            if (ret1.is<std::string>()) cpstr = ret1.as<std::string>();
            if (cpstr == "print")
            {
                antlrcpp::Any ret2;
                ret2 = visit(ctx->trailer());
                if (!(ret2.is<int>() && ret2.as<int>() == 7))
                {
                    std::vector<antlrcpp::Any> toprint;
                    toprint = ret2.as<std::vector<antlrcpp::Any>>();
                    for (int i = 0; i < toprint.size(); ++i)
                    {
                        if (toprint[i].is<std::vector<antlrcpp::Any>>())
                        {
                            //std::cout<<"prinnt\n";
                            std::vector<antlrcpp::Any> in_toprint;
                            in_toprint = toprint[i].as<std::vector<antlrcpp::Any>>();
                            for (int j = 0; j < in_toprint.size(); ++j)
                            {
                                if (in_toprint[j].is<std::string>())
                                {
                                    std::string str = in_toprint[j].as<std::string>();
                                    if (str[0] != '\'' && str[0] != '\"')
                                        in_toprint[j] = paraments[str];
                                }
                                if (in_toprint[j].is<std::string>())
                                {
                                    std::string str = in_toprint[j].as<std::string>();
                                    str.erase(str.length() - 1, 1);
                                    str.erase(0, 1);
                                    std::cout << str;
                                }
                                if (in_toprint[j].is<bigInteger>())
                                    std::cout << in_toprint[j].as<bigInteger>();
                                if (in_toprint[j].is<int>())
                                {
                                    if (in_toprint[j].as<int>() == -1) std::cout << "None";
                                    if (in_toprint[j].as<int>() == 1) std::cout << "True";
                                    if (in_toprint[j].as<int>() == 0) std::cout << "False";
                                }
                                if (in_toprint[j].is<double>())
                                    std::cout << std::fixed << std::setprecision(6) << toprint[i].as<double>();
                                if (i != toprint.size() - 1 || j != in_toprint.size() - 1) {
                                    std::cout << ' ';
                                }
                            }
                        }
                        if (toprint[i].is<std::string>())
                        {
                            std::string str = toprint[i].as<std::string>();
                            if (str[0] != '\'' && str[0] != '\"')
                                toprint[i] = paraments[str];
                        }
                        if (toprint[i].is<std::string>())
                        {
                            std::string str = toprint[i].as<std::string>();
                            str.erase(str.length() - 1, 1);
                            str.erase(0, 1);
                            std::cout << str;
                        }
                        if (toprint[i].is<bigInteger>())
                            std::cout << toprint[i].as<bigInteger>();
                        if (toprint[i].is<int>()) {
                            if (toprint[i].as<int>() == -1) std::cout << "None";
                            if (toprint[i].as<int>() == 1) std::cout << "True";
                            if (toprint[i].as<int>() == 0) std::cout << "False";
                        }
                        if (toprint[i].is<double>())
                            std::cout << std::fixed << std::setprecision(6) << toprint[i].as<double>();
                        if (i != toprint.size() - 1) std::cout << ' ';
                    }
                }
                std::cout << "\r\n";
                return 0;
            }
            if (cpstr == "int")
            {
                antlrcpp::Any ret2;
                bigInteger toint;
                std::string thestr;
                ret2 = visit(ctx->trailer());
                while (ret2.is<std::vector<antlrcpp::Any>>())
                    ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
                if (ret2.is<std::string>())
                {
                    std::string strret2 = ret2.as<std::string>();
                    if (strret2[0] != '\'' && strret2[0] != '\"')
                        ret2 = paraments[strret2];
                }
                if (ret2.is<std::string>())
                {
                    thestr = ret2.as<std::string>();
                    thestr.erase(0,1);
                    thestr.erase(thestr.size()-1,1);
                    char *p = &thestr[0];
                    toint = p;
                    ret2 = toint;
                }
                if (ret2.is<double>())
                    ret2 = (int)ret2.as<double>();
                if (ret2.is<int>())
                {
                    thestr = std::to_string(ret2.as<int>());
                    char *p = &thestr[0];
                    toint = p;
                    ret2 = toint;
                }
                return ret2;
            }
            if (cpstr == "float")
            {
                antlrcpp::Any ret2;
                ret2 = visit(ctx->trailer());
                while (ret2.is<std::vector<antlrcpp::Any>>())
                    ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
                if (ret2.is<std::string>())
                {
                    std::string strret2 = ret2.as<std::string>();
                    if (strret2[0] != '\'' && strret2[0] != '\"')
                        ret2 = paraments[strret2];
                }
                if (ret2.is<bigInteger>())
                    ret2 = (double)ret2.as<bigInteger>();
                if (ret2.is<std::string>())
                {
                    std::string thestr = ret2.as<std::string>();
                    thestr.erase(0,1);
                    thestr.erase(thestr.size()-1,1);
                    char *p = &thestr[0];
                    ret2 = atof(p);
                }
                if (ret2.is<int>())
                    ret2 = (double)ret2.as<int>();
                return ret2;
            }
            if (cpstr == "str")
            {
                antlrcpp::Any ret2;
                std::string yinhao = "\'";
                ret2 = visit(ctx->trailer());
                while (ret2.is<std::vector<antlrcpp::Any>>())
                    ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
                if (ret2.is<std::string>())
                {
                    std::string strret2 = ret2.as<std::string>();
                    if (strret2[0] != '\'' && strret2[0] != '\"')
                        ret2 = paraments[strret2];
                }
                if (ret2.is<bigInteger>())
                {
                    ret2 = (std::string)ret2.as<bigInteger>();
                }
                if (ret2.is<int>())
                {
                    int theint = ret2.as<int>();
                    if (theint == 1) ret2 = std::string("True");
                    if (theint == 0) ret2 = std::string("False");
                    if (theint == -1) ret2 = std::string("None");
                }
                if (ret2.is<double>())
                    ret2 = std::to_string(ret2.as<double>());
                std::string ans = yinhao + ret2.as<std::string>() + yinhao;
                ret2 = ans;
                return ret2;
            }
            if (cpstr == "bool")
            {
                antlrcpp::Any ret2;
                int thebool;
                ret2 = visit(ctx->trailer());
                while (ret2.is<std::vector<antlrcpp::Any>>())
                    ret2 = ret2.as<std::vector<antlrcpp::Any>>()[0];
                if (ret2.is<std::string>())
                {
                    std::string strret2 = ret2.as<std::string>();
                    if (strret2[0] != '\'' && strret2[0] != '\"')
                        ret2 = paraments[strret2];
                }
                if (ret2.is<bigInteger>())
                {
                    ret2 = (bool)ret2.as<bigInteger>();
                    thebool = ret2.as<bool>();
                }
                if (ret2.is<double>())
                {
                    ret2 = (bool)ret2.as<double>();
                    thebool = ret2.as<bool>();
                }
                if (ret2.is<std::string>())
                {
                    std::string strret2 = ret2.as<std::string>();
                    strret2.erase(0,1);
                    strret2.erase(strret2.size()-1,1);
                    if (strret2 == "") thebool = 0;
                    else thebool = 1;
                }
                return thebool;
            }
            else
            {
                std::vector<std::string> tov;
                to_get_value.push(tov);
                std::vector<antlrcpp::Any> toov;
                return_value.push(toov);
                level.push(paraments);
                if (!is_global)  {
                    global_variables = paraments;
                }
                tempmap.push(global_variables);
                ++is_global;
                //std::cout<<return_value.top().as<int>()<<'\n';
                Python3Parser::ParametersContext* temp_parameters = funcparameter[cpstr];
                if (temp_parameters->typedargslist() != nullptr) {
                    visit(temp_parameters->typedargslist());
                    antlrcpp::Any funcrett = visit(ctx->trailer());
                    std::vector<antlrcpp::Any> this_value;
                    if (funcrett.is<std::vector<antlrcpp::Any>>()) {
                        this_value = funcrett.as<std::vector<antlrcpp::Any>>();
                    }
                    for (int i = 0;i < this_value.size();++i)
                    {
                        bool flag = false;
                        std::string str = to_get_value.top()[i];
                        if (this_value[i].is<int>())
                        {
                            if(this_value[i].as<int>() == 7) flag = true;
                        }
                        if (!flag) {
                            if (this_value[i].is<std::string>())
                            {
                                std::string convert = this_value[i].as<std::string>();
                                if (convert[0] != '\'' && convert[0] != '\"')
                                    this_value[i] = paraments[convert];
                            }
                            tempmap.top()[str] = this_value[i];
                        }
                    }
                }
                to_get_value.pop();
                paraments = tempmap.top();
                std::vector<antlrcpp::Any> tempreturn;
                visit(funcsuite[cpstr]);
                tempreturn = return_value.top();
                //if (tempreturn[0].is<std::string>())std::cout<<tempreturn[0].as<std::string>()<<'\n';
                /*if (tempreturn.is<std::vector<antlrcpp::Any>>()) {
                    std::vector<antlrcpp::Any> convert = tempreturn.as<std::vector<antlrcpp::Any>>();
                    for (int i = 0;i < convert.size();++i)
                    {
                        if (convert[i].is<std::string>())
                        {
                            std::string strpara = convert[i].as<std::string>();
                            if (strpara[0] != '\'' && strpara[0] != '\"')
                                convert[i] = paraments[strpara];
                        }
                    }
                    tempreturn = convert;
                }*/
                paraments = level.top();
                return_value.pop();
                level.pop();
                tempmap.pop();
                --is_global;
                if(!tempreturn.empty()) return tempreturn;
                else {
                    return 7;
                }
            }
        }
        else return visit(ctx->atom());
        return 0;
    }

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *ctx) override {
        //std::cout<<"trailer"<<'\n';
        if (ctx->arglist() != nullptr) return visit(ctx->arglist());
        else return 7;
    }

    antlrcpp::Any visitAtom(Python3Parser::AtomContext *ctx) override {
        //std::cout<<"atom"<<'\n';
        antlrcpp::Any temp;
        if (ctx->NAME())
        {
            temp = ctx->NAME()->toString();
            std::string name = temp;
            return name;
        }
        if (ctx->STRING(0))
        {
            std::string str;
            for (int i = 0;i < ctx->STRING().size();++i)
            {
                std::string tempstr;
                tempstr = ctx->STRING(i)->toString();
                if (i != ctx->STRING().size() - 1) tempstr.erase(tempstr.length() - 1,1);
                if (i != 0) tempstr.erase(0,1);
                str += tempstr;
            }
            return str;
        }
        if (ctx->NUMBER())
        {
            std::string str = ctx->NUMBER()->toString();
            char *st = &str[0];
            for (int i = 0;i < str.size();++i)
            {
                if (str[i] == '.')
                {
                    double doubleans;
                    doubleans = atof(st);
                    return doubleans;
                }
            }
            bigInteger tempbigint(st);
            return tempbigint;
        }
        if (ctx->NONE()) return (int)-1;
        if (ctx->TRUE()) return (int)1;
        if (ctx->FALSE()) return (int)0;
        if (ctx->test()) return visit(ctx->test());
        return visitChildren(ctx);
    }

    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *ctx) override {
        //std::cout<<"visitTestlist"<<'\n';
        std::vector<antlrcpp::Any> temp;
        antlrcpp::Any ret;
        int i = 0;
        for (;i < ctx->test().size();++i)
        {
            ret = visit(ctx->test(i));
            if (ret.is<std::vector<antlrcpp::Any>>())
            {
                std::vector<antlrcpp::Any> temp2 = ret.as<std::vector<antlrcpp::Any>>();
                for (int j = 0;j < temp2.size();++j)
                {
                    temp.push_back(temp2[j]);
                }
            }
            else temp.push_back(ret);
        }
        return temp;
    }

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *ctx) override {
        //std::cout<<"arglist"<<'\n';
        std::vector<antlrcpp::Any> tempvec;
        for (int i = 0;i < ctx->argument().size();++i)
        {
            antlrcpp::Any ret = visit(ctx->argument(i));
            if (ret.is<std::vector<antlrcpp::Any>>())
            {
                //std::cout<<"func\n";
                std::vector<antlrcpp::Any> t = ret.as<std::vector<antlrcpp::Any>>();
                //std::cout<<t[0].as<std::string>()<<t.size()<<'\n';
                for (int j = 0;j < t.size();++j)
                {
                    tempvec.push_back(t[j]);
                }
            }
            else tempvec.push_back(ret);
        }
        return tempvec;
    }

    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *ctx) override {
        //std::cout<<"argument\n";
        antlrcpp::Any ret = visit(ctx->test());
        if (ret.is<std::vector<antlrcpp::Any>>())
        {
            std::vector<antlrcpp::Any> pushvec = ret.as<std::vector<antlrcpp::Any>>();
            std::vector<antlrcpp::Any> jietao;
            for (int i = 0;i < pushvec.size();++i)
            {
                if (pushvec[i].is<std::vector<antlrcpp::Any>>())
                {
                    std::vector<antlrcpp::Any> in = pushvec[i].as<std::vector<antlrcpp::Any>>();
                    for (int j = 0;j < in.size();++j)
                    {
                        jietao.push_back(in[j]);
                    }
                }
                else jietao.push_back(pushvec[i]);
            }
            ret = jietao;
        }
        if (ret.is<std::string>())
        {
            //std::cout<<"string\t"<<ret.as<std::string>()<<'\n';
            std::string retstr = ret.as<std::string>();
            if (retstr[0] != '\'' && retstr[0] != '\"')
                ret = paraments[retstr];
            //if (ret.is<bigInteger>()) std::cout<<"this\tbigint"<<ret.as<bigInteger>()<<"\n";
        }
        //if (ret.is<bigInteger>()) std::cout<<"BIGINT\t"<<ret.as<bigInteger>()<<'\n';
        if (ctx->NAME() != nullptr)
        {
            //std::cout<<"?????\n";
            std::string str = ctx->NAME()->toString();
            if (ret.is<std::vector<antlrcpp::Any>>())
                ret = ret.as<std::vector<antlrcpp::Any>>()[0];
            tempmap.top()[str] = ret;
            return 7;
        }
        else {
            return ret;
        }
    }

};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
