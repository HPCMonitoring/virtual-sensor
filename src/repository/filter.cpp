#include "repository/filter.h"

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

        return std::regex_search(compareValue.begin(), compareValue.end(), literalRegex);
    }
    if (this->op == LT_OP || this->op == LTE_OP || this->op == GT_OP || this->op == GTE_OP)
    {
        std::string compareValueStr;
        if (this->operand == N_VMU)
            compareValueStr = proc->getVirtualMemory();
        else if (this->operand == N_PMU)
            compareValueStr = proc->getPhysicalMemory();
        else if (this->operand == N_CPUT)
            compareValueStr = proc->getCpuTime();
        else if (this->operand == N_CPUU)
            compareValueStr = proc->getCpuUsage();
        else if (this->operand == N_NETIN)
            compareValueStr = proc->getNetworkIn();
        else if (this->operand == N_NETOUT)
            compareValueStr = proc->getNetworkOut();
        else if (this->operand == N_IOR)
            compareValueStr = proc->getReadKBs();
        else if (this->operand == N_IOW)
            compareValueStr = proc->getWriteKBs();

        if(compareValueStr == "null") return false;
        
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
    // Only find if PID is under AND condition
    if (this->op == OR_OP)
        return "";

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
    if (this->op == AND_OP)
    {
        for (size_t i = 0; i < numSubOfExprs; ++i)
        {
            if (!this->subExprs.at(i)->validate(proc))
                return false;
        }
        return true;
    }
    if (this->op == OR_OP)
    {
        for (size_t i = 0; i < numSubOfExprs; ++i)
        {
            if (this->subExprs.at(i)->validate(proc))
                return true;
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
    while (issLexicals >> std::quoted(temp))
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
std::string Filter::iterateProc(Process *proc) const
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
            jsonAttrStr.append(proc->getVirtualMemory());
        else if (this->projection.at(i).name == N_PMU)
            jsonAttrStr.append(proc->getPhysicalMemory());
        else if (this->projection.at(i).name == N_CPUT)
            jsonAttrStr.append(proc->getCpuTime());
        else if (this->projection.at(i).name == N_CPUU)
            jsonAttrStr.append(proc->getCpuUsage());
        else if (this->projection.at(i).name == N_NETIN)
            jsonAttrStr.append(proc->getNetworkIn());
        else if (this->projection.at(i).name == N_NETOUT)
            jsonAttrStr.append(proc->getNetworkOut());
        else if (this->projection.at(i).name == N_IOR)
            jsonAttrStr.append(proc->getReadKBs());
        else if (this->projection.at(i).name == N_IOW)
            jsonAttrStr.append(proc->getWriteKBs());

        if (i != numOfAttrs - 1)
            jsonAttrStr.push_back(',');

        result.append(jsonAttrStr);
    }

    result.push_back('}');
    std::transform(result.begin(), result.end(), result.begin(),
                   [](char c)
                   { return std::iscntrl(c) ? ' ' : c; });
    return result;
}

std::string Filter::iterateNetworkInterface(const NetworkInterface *networkInterface) const
{
    size_t numOfAttrs = this->projection.size();
    std::ostringstream oss;
    oss << '{';

    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string fieldName = this->projection.at(i).alias.length() > 0 ? this->projection.at(i).alias : this->projection.at(i).name;

        oss << '\"' << fieldName << "\":";

        if (this->projection.at(i).name == "name")
            oss << '\"' << networkInterface->getName() << '\"';
        else if (this->projection.at(i).name == "receive")
        {
            auto receiveData = networkInterface->getReceiveData();
            oss << "{";
            oss << "\""
                << "bytes"
                << "\":" << receiveData->bytes << ',';
            oss << "\""
                << "packets"
                << "\":" << receiveData->packets << ',';
            oss << "\""
                << "errors"
                << "\":" << receiveData->errors << ',';
            oss << "\""
                << "dropped"
                << "\":" << receiveData->dropped << ',';
            oss << "\""
                << "fifo"
                << "\":" << receiveData->fifo << ',';
            oss << "\""
                << "frame"
                << "\":" << receiveData->frame << ',';
            oss << "\""
                << "compressed"
                << "\":" << receiveData->compressed << ',';
            oss << "\""
                << "multicast"
                << "\":" << receiveData->multicast;
            oss << "}";
        }
        else if (this->projection.at(i).name == "transmit")
        {
            auto transmitData = networkInterface->getTransmitData();
            oss << "{";
            oss << "\""
                << "bytes"
                << "\":" << transmitData->bytes << ',';
            oss << "\""
                << "packets"
                << "\":" << transmitData->packets << ',';
            oss << "\""
                << "errors"
                << "\":" << transmitData->errors << ',';
            oss << "\""
                << "dropped"
                << "\":" << transmitData->dropped << ',';
            oss << "\""
                << "fifo"
                << "\":" << transmitData->fifo << ',';
            oss << "\""
                << "colls"
                << "\":" << transmitData->colls << ',';
            oss << "\""
                << "carrier"
                << "\":" << transmitData->carrier << ',';
            oss << "\""
                << "compressed"
                << "\":" << transmitData->compressed;
            oss << "}";
        }
        if (i != numOfAttrs - 1)
            oss << ',';
    }

    oss << '}';
    return oss.str();
}

void Filter::print()
{
    std::cout << "Data type: " << this->datatype << '\n';
    std::cout << "Fields: "
              << "<\n";
    std::cout << '\t' << this->projection.at(0).name;
    if (this->projection.at(0).alias.length() > 0)
        std::cout << " -> " << this->projection.at(0).alias;
    for (size_t i = 1; i < this->projection.size(); ++i)
    {
        std::cout << ",\n\t" << this->projection.at(i).name;
        if (this->projection.at(i).alias.length() > 0)
            std::cout << " -> " << this->projection.at(i).alias;
    }
    std::cout << "\n>\n";

    std::cout << "Conditions: ";
    this->selection->print();
    std::cout << '\n';
}

std::string Filter::iterateMemory(const Memory *memory) const
{
    size_t numOfAttrs = this->projection.size();
    std::ostringstream oss;
    oss << '{';

    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string fieldName = this->projection.at(i).alias.length() > 0 ? this->projection.at(i).alias : this->projection.at(i).name;

        oss << '\"' << fieldName << "\":";

        if (this->projection.at(i).name == "total")
            oss << memory->total;
        else if (this->projection.at(i).name == "free")
            oss << memory->free;
        else if (this->projection.at(i).name == "available")
            oss << memory->available;
        else if (this->projection.at(i).name == "buffers")
            oss << memory->buffers;
        else if (this->projection.at(i).name == "cached")
            oss << memory->cached;
        else if (this->projection.at(i).name == "swapTotal")
            oss << memory->swapTotal;
        else if (this->projection.at(i).name == "swapFree")
            oss << memory->swapFree;
        else if (this->projection.at(i).name == "swapCached")
            oss << memory->swapCached;

        if (i != numOfAttrs - 1)
            oss << ',';
    }
    oss << '}';
    return oss.str();
};

std::string Filter::iterateCpu(const Cpu *cpu) const
{
    size_t numOfAttrs = this->projection.size();
    std::ostringstream oss;
    oss << '{';
    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string fieldName = this->projection.at(i).alias.length() > 0 ? this->projection.at(i).alias : this->projection.at(i).name;
        oss << '\"' << fieldName << "\":";

        if (this->projection.at(i).name == "order")
            oss << cpu->order;
        else if (this->projection.at(i).name == "user")
            oss << cpu->user;
        else if (this->projection.at(i).name == "nice")
            oss << cpu->nice;
        else if (this->projection.at(i).name == "system")
            oss << cpu->system;
        else if (this->projection.at(i).name == "idle")
            oss << cpu->idle;
        else if (this->projection.at(i).name == "iowait")
            oss << cpu->iowait;
        else if (this->projection.at(i).name == "irq")
            oss << cpu->irq;
        else if (this->projection.at(i).name == "softirq")
            oss << cpu->softirq;
        else if (this->projection.at(i).name == "steal")
            oss << cpu->steal;
        else if (this->projection.at(i).name == "guest")
            oss << cpu->guest;
        else if (this->projection.at(i).name == "guestNice")
            oss << cpu->guestNice;

        if (i != numOfAttrs - 1)
            oss << ',';
    }
    oss << '}';
    return oss.str();
}

std::string Filter::iterateIO(const IOStat *io) const
{
    size_t numOfAttrs = this->projection.size();
    std::ostringstream oss;
    oss << '{';
    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string fieldName = this->projection.at(i).alias.length() > 0 ? this->projection.at(i).alias : this->projection.at(i).name;
        oss << '\"' << fieldName << "\":";

        if (this->projection.at(i).name == "device")
            oss << '\"' << io->device << '\"';
        else if (this->projection.at(i).name == "tps")
            oss << io->tps;
        else if (this->projection.at(i).name == "readPerSec")
            oss << io->readPerSec;
        else if (this->projection.at(i).name == "read")
            oss << io->read;
        else if (this->projection.at(i).name == "writePerSec")
            oss << io->tps;
        else if (this->projection.at(i).name == "write")
            oss << io->write;
        else if (this->projection.at(i).name == "discardPerSec")
            oss << io->discardPerSec;
        else if (this->projection.at(i).name == "discard")
            oss << io->discard;

        if (i != numOfAttrs - 1)
            oss << ',';
    }
    oss << '}';
    return oss.str();
}

std::string Filter::iterateDiskUsage(const DiskUsage *disk) const
{
    size_t numOfAttrs = this->projection.size();
    std::ostringstream oss;
    oss << '{';
    for (size_t i = 0; i < numOfAttrs; ++i)
    {
        std::string fieldName = this->projection.at(i).alias.length() > 0 ? this->projection.at(i).alias : this->projection.at(i).name;
        oss << '\"' << fieldName << "\":";

        if (this->projection.at(i).name == "filesystem")
            oss << '\"' << disk->filesystem << '\"';
        else if (this->projection.at(i).name == "size")
            oss << '\"' << disk->size << '\"';
        else if (this->projection.at(i).name == "used")
            oss << '\"' << disk->used << '\"';
        else if (this->projection.at(i).name == "available")
            oss << '\"' << disk->available << '\"';
        else if (this->projection.at(i).name == "usedPercentage")
            oss << '\"' << disk->usedPercentage << '\"';
        else if (this->projection.at(i).name == "mountedOn")
            oss << '\"' << disk->mountedOn << '\"';

        if (i != numOfAttrs - 1)
            oss << ',';
    }
    oss << '}';
    return oss.str();
}

Filter::~Filter()
{
    delete this->selection;
}