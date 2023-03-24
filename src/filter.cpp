#include "filter.h"

RelationalExpr::RelationalExpr(const std::string &op, const std::string &operand, const std::string &literal)
{
    this->op = op;
    this->operand = operand;
    this->literal = literal;
}
bool RelationalExpr::check()
{
    return true;
}
void RelationalExpr::print()
{
    std::cout << '(' << this->op << ' ' << this->operand << ' ' << this->literal << ')';
}

LogicalExpr::LogicalExpr(const std::string &op, const ushort numOfSubExprs)
{
    this->op = op;
    this->numSubOfExprs = numOfSubExprs;
}
bool LogicalExpr::check()
{
    return true;
}
void LogicalExpr::print()
{
    std::cout << '(' << this->op << ' ' << this->numSubOfExprs << ' ';
    for (size_t i = 0; i < this->subExprs.size(); ++i)
    {
        this->subExprs.at(i)->print();
    }
    std::cout << ')';
}

LogicalExpr::~LogicalExpr()
{
    for (size_t i = 0; i < this->subExprs.size(); ++i)
    {
        delete this->subExprs[i];
    }
}

Filter::Filter(const std::string &datatype, const std::string &fields, const std::string &whereCondition)
{
    this->datatype = datatype;

    std::istringstream issFields(fields);
    std::string temp;
    while (issFields >> temp)
        this->fields.push_back(temp);

    std::istringstream issLexicals(whereCondition);
    std::stack<Expr *> exprStack;
    std::stack<std::string> lexicalStack;

    while (issLexicals >> temp)
        lexicalStack.push(temp);

    std::stack<std::string> operandStack;
    while (!lexicalStack.empty())
    {
        std::string lexical = lexicalStack.top();
        lexicalStack.pop();

        if (lexical == AND_OP || lexical == OR_OP)
        {
            int numOfSubExprs = std::stoi(operandStack.top());
            operandStack.pop();
            LogicalExpr *logicalExpr = new LogicalExpr(lexical, numOfSubExprs);

            while (numOfSubExprs > 0)
            {
                logicalExpr->subExprs.push_back(exprStack.top());
                exprStack.pop();
                --numOfSubExprs;
            }

            exprStack.push(logicalExpr);
        }
        else if (lexical == EQ_OP || lexical == LT_OP || lexical == LTE_OP || lexical == GT_OP || lexical == GTE_OP || lexical == LIKE_OP)
        {
            std::string operand = operandStack.top();
            operandStack.pop();
            std::string literal = operandStack.top();
            operandStack.pop();

            RelationalExpr *relationalExpr = new RelationalExpr(lexical, operand, literal);
            exprStack.push(relationalExpr);
        }
        else
        {
            operandStack.push(lexical);
        }
    }

    this->whereCondition = exprStack.top();
    exprStack.pop();
    std::cout << "Stack size: " << exprStack.size() << '\n';
}
void Filter::print()
{
    std::cout << "Data type: " << this->datatype << std::endl;
    std::cout << "Fields: "
              << "<";
    std::cout << this->fields.at(0);
    for (size_t i = 1; i < this->fields.size(); ++i)
    {
        std::cout << ", " << this->fields.at(i);
    }
    std::cout << '>' << std::endl;

    std::cout << "Conditions: ";
    this->whereCondition->print();
    std::cout << std::endl;
}
Filter::~Filter()
{
    delete this->whereCondition;
}