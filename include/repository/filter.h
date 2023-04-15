#ifndef __FILTER_H__
#define __FILTER_H__

#include "main.h"
#include "schemas/process.h"
#include "schemas/network_interface.h"

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
    Attribute(const std::string & name) {
        this->name = name;
        this->alias = "";
    }
};

class Expr
{
protected:
    std::string op;

public:
    virtual void print() const = 0;
    virtual std::string findPid() = 0;
    virtual bool validate(Process*) const = 0;
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
    void print() const;
    bool validate(Process*) const;
    std::string findPid();
};

class LogicalExpr : public Expr
{
public:
    ushort numSubOfExprs;
    std::vector<Expr *> subExprs;

public:
    LogicalExpr(const std::string &op, const ushort numOfSubExprs);
    void print() const;
    std::string findPid();
    bool validate(Process*) const;
    ~LogicalExpr();
};

class Filter
{
public:
    std::string datatype;
    std::vector<Attribute> projection;
    Expr *selection;

private:
    Filter();

public:
    Filter(const std::string &datatype, const std::vector<Attribute> &projection);
    Filter(const std::string &datatype, const std::vector<Attribute> &projection, const std::string &selection);
    std::string iterateProc(Process *) const;
    std::string iterateNetworkInterface(const NetworkInterface*) const;
    void print();
    ~Filter();
};

#endif