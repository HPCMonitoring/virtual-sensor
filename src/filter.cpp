#include "filter.h"

template <typename T>
inline bool compare(const T &a, const T &b, const std::string &op)
{
    if (op == LT_OP)
        return a < b;
    if (op == LTE_OP)
        return a <= b;
    if (op == GT_OP)
        return a > b;
    if (op == GTE_OP)
        return a >= b;
    return false;
}

RelationalExpr::RelationalExpr(const std::string &op, const std::string &operand, const std::string &literal)
{
    this->op = op;
    this->operand = operand;
    this->literal = literal;
}
std::string RelationalExpr::findPid()
{
    if (this->operand == "pid")
        return this->literal;
    return "";
}
bool RelationalExpr::validate(Process *proc) const
{
    if (this->op == EQ_OP)
    {
        if (this->operand == N_PID)
            return this->literal == proc->getPid();
        if (this->operand == N_PPID)
            return this->literal == proc->getParentPid();
        if (this->operand == N_GID)
            return this->literal == proc->getGid();
        if (this->operand == N_UID)
            return this->literal == proc->getUid();
        return false;
    }
    if (this->op == LIKE_OP)
    {
        std::regex literalRegex(this->literal);
        std::string compareValue = proc->getName();
        if (this->operand == N_NAME)
            compareValue = proc->getName();
        else if (this->operand == N_CMD)
            compareValue = proc->getCommand();
        else if (this->operand == N_EXEC_PATH)
            compareValue = proc->getExecutePath();

        return std::regex_match(compareValue.begin(), compareValue.end(), literalRegex);
    }
    if (this->op == LT_OP || this->op == LTE_OP || this->op == GT_OP || this->op == GTE_OP)
    {
        std::string compareValueStr;
        if (this->operand == N_VMU)
            compareValueStr = proc->getVirtualMemoryUsage();
        else if (this->operand == N_PMU)
            compareValueStr = proc->getPhysicalMemoryUsage();
        else if (this->operand == N_CPUT)
            compareValueStr = proc->getCpuTime();
        else if (this->operand == N_CPUU)
            compareValueStr = proc->getCpuUsage();
        else if (this->operand == N_NETIN)
            compareValueStr = proc->getNetworkInBandwidth();
        else if (this->operand == N_NETOUT)
            compareValueStr = proc->getNetworkOutBandwidth();
        else if (this->operand == N_IOR)
            compareValueStr = proc->getIoRead();
        else if (this->operand == N_IOW)
            compareValueStr = proc->getIoWrite();
        
        const double compareValue = compareValueStr.length() > 0 ? std::stod(compareValueStr) : -1.0;
        const double literalValue = std::stod(this->literal);
        return compare<double>(compareValue, literalValue, this->op);
    }

    return false;
}
void RelationalExpr::print() const
{
    std::cout << '(' << this->op << ' ' << this->operand << ' ' << this->literal << ')';
}

LogicalExpr::LogicalExpr(const std::string &op, const ushort numOfSubExprs)
{
    this->op = op;
    this->numSubOfExprs = numOfSubExprs;
}
std::string LogicalExpr::findPid()
{
    for (size_t i = 0; i < this->subExprs.size(); ++i)
    {
        const std::string pid = this->subExprs.at(i)->findPid();
        if (pid.length() != 0)
        {
            return pid;
        }
    }
    return "";
}
bool LogicalExpr::validate(Process *proc) const
{
    if(this->op == AND_OP) {
        for(size_t i = 0; i < numSubOfExprs; ++i) {
            if(!this->subExprs.at(i)->validate(proc)) return false;
        }
        return true;
    }
    if(this->op == OR_OP) {
        for(size_t i = 0; i < numSubOfExprs; ++i) {
            if(this->subExprs.at(i)->validate(proc)) return true;
        }
        return false;
    }
    return false;
}
void LogicalExpr::print() const
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

Filter::Filter(const std::string &datatype, const std::vector<Attribute> &projection)
{
    this->datatype = datatype;
    this->projection = projection;
    this->selection = nullptr;
}
Filter::Filter(const std::string &datatype, const std::vector<Attribute> &projection, const std::string &selection)
{
    this->datatype = datatype;
    this->projection = projection;

    std::istringstream issLexicals(selection);
    std::stack<Expr *> exprStack;
    std::stack<std::string> lexicalStack;

    std::string temp;
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

    this->selection = exprStack.top();
    exprStack.pop();
}
std::string Filter::iterate(Process *proc) const
{
    size_t numOfAttrs = this->projection.size();
    std::string result;
    result.push_back('{');

    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string jsonAttrStr = "";
        std::string fieldName = this->projection.at(i).alias.length() > 0 ? this->projection.at(i).alias : this->projection.at(i).name;

        jsonAttrStr.push_back('\"');
        jsonAttrStr.append(fieldName);
        jsonAttrStr.append("\":");

        if (this->projection.at(i).name == N_NAME)
        {
            jsonAttrStr.push_back('\"');
            jsonAttrStr.append(proc->getName());
            jsonAttrStr.push_back('\"');
        }
        else if (this->projection.at(i).name == N_PID)
            jsonAttrStr.append(proc->getPid());
        else if (this->projection.at(i).name == N_PPID)
            jsonAttrStr.append(proc->getParentPid());
        else if (this->projection.at(i).name == N_UID)
            jsonAttrStr.append(proc->getUid());
        else if (this->projection.at(i).name == N_GID)
            jsonAttrStr.append(proc->getGid());
        else if (this->projection.at(i).name == N_EXEC_PATH)
        {
            jsonAttrStr.push_back('\"');
            jsonAttrStr.append(proc->getExecutePath());
            jsonAttrStr.push_back('\"');
        }
        else if (this->projection.at(i).name == N_CMD)
        {
            jsonAttrStr.push_back('\"');
            jsonAttrStr.append(proc->getCommand());
            jsonAttrStr.push_back('\"');
        }
        else if (this->projection.at(i).name == N_VMU)
            jsonAttrStr.append(proc->getVirtualMemoryUsage());
        else if (this->projection.at(i).name == N_PMU)
            jsonAttrStr.append(proc->getPhysicalMemoryUsage());
        else if (this->projection.at(i).name == N_CPUT)
            jsonAttrStr.append(proc->getCpuTime());
        else if (this->projection.at(i).name == N_CPUU)
            jsonAttrStr.append(proc->getCpuUsage());
        else if (this->projection.at(i).name == N_NETIN)
            jsonAttrStr.append(proc->getNetworkInBandwidth());
        else if (this->projection.at(i).name == N_NETOUT)
            jsonAttrStr.append(proc->getNetworkOutBandwidth());
        else if (this->projection.at(i).name == N_IOR)
            jsonAttrStr.append(proc->getIoRead());
        else if (this->projection.at(i).name == N_IOW)
            jsonAttrStr.append(proc->getIoWrite());

        if (i != numOfAttrs - 1)
            jsonAttrStr.push_back(',');

        result.append(jsonAttrStr);
    }

    result.push_back('}');
    return result;
}
inline void Filter::print()
{
    std::cout << "Data type: " << this->datatype << std::endl;
    std::cout << "Fields: "
              << "<";
    std::cout << this->projection.at(0).name << "as" << this->projection.at(0).alias;
    for (size_t i = 1; i < this->projection.size(); ++i)
    {
        std::cout << ", " << this->projection.at(i).name << "as" << this->projection.at(i).alias;
    }
    std::cout << '>' << std::endl;

    std::cout << "Conditions: ";
    this->selection->print();
    std::cout << std::endl;
}
Filter::~Filter()
{
    delete this->selection;
}