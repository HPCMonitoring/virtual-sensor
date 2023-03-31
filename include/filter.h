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

#define PROCESS "process"
#define NETWORK_INTERFACE "network_interface"
#define MEMORY "memory"
#define CPU "cpu"
#define IO "io"
#define DISK "disk"

class Expr
{
protected:
    std::string op;

public:
    virtual bool check() = 0;
    virtual void print() = 0;
    virtual std::string findPid() = 0;
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
    std::string findPid() {
        if(this->operand == "pid") return this->literal;
        return "";
    }
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
    std::string findPid() {
        for(size_t i = 0 ; i < this->subExprs.size(); ++i) {
            const std::string pid = this->subExprs.at(i)->findPid();
            if(pid.length() != 0) {
                return pid;
            }
        }
        return "";
    }
    ~LogicalExpr();
};

class Attribute
{
public:
    std::string name;
    std::string alias;
    Attribute(const std::string &name, const std::string &alias)
    {
        this->name = name;
        this->alias = alias;
    }
};

class Filter
{
public:
    std::string datatype;
    std::vector<Attribute> attrs;
    Expr *whereCondition;

private:
    Filter();

public:
    Filter(const std::string &datatype, const std::vector<Attribute> &attrs);
    Filter(const std::string &datatype, const std::vector<Attribute> &attrs, const std::string &whereCondition);
    void print();
    ~Filter();
};

#endif