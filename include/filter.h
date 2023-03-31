#ifndef __FILTER_H__
#define __FILTER_H__

#include "main.h"

#define AND_OP "&&"
#define OR_OP "||"
#define EQ_OP "=="
#define LT_OP "<"
#define GT_OP ">"
#define LTE_OP "<="
#define GTE_OP ">="
#define LIKE_OP "%="

class Expr
{
protected:
    std::string op;

public:
    virtual bool check() = 0;
    virtual void print() = 0;
    virtual ~Expr(){};
};

class RelationalExpr : public Expr
{
public:
    std::string op;
    std::string operand;
    std::string literal;

public:
    RelationalExpr(const std::string &op, const std::string &operand, const std::string &literal);
    bool check();
    void print();
};

class LogicalExpr : public Expr
{
public:
    ushort numSubOfExprs;
    std::vector<Expr *> subExprs;

public:
    LogicalExpr(const std::string &op, const ushort numOfSubExprs);
    bool check();
    void print();
    ~LogicalExpr();
};

class Filter
{
public:
    std::string datatype;
    std::vector<std::string> fields;
    Expr *whereCondition;

private:
    Filter();

public:
    Filter(const std::string &datatype, const std::string &fields, const std::string &whereCondition);
    void print();
    ~Filter();
};

#endif