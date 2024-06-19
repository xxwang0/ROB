#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <windows.h>
#include <cstdlib>
int t;

std::queue<int> q; // 指令队列（发射队列）
struct st
{
    int n, step;
};
struct instr // 存放指令的各个细节
{
    std::string op;
    int fi, fj, fk, cycle, device, rj, rk, qj, qk;
    int issue, read, execution, write;
    std::string name;
} instruction[100 + 1];

std::queue<int> reg_q[30 + 1]; // 存放需要该寄存器结果的指令

int reg[31 + 1];    // 存放写该寄存器的指令
int device[5 + 1];   // 存放占用该功能部件的指令
std::set<int> step[6]; // 存放各个阶段的指令队列
std::set<int>::iterator it, ite; // 迭代器，用来枚举
std::map<std::string, int> map; // 用来做映射，将数字与具体指代指令联系
std::map<int, std::string> mapp; // 用来做映射，将数字与具体指代指令联系
std::string m[3 + 1][30 + 1]; // 用来做映射，将数字与具体指代指令联系

// 初始化
inline void init()
{
    map["LD"] = 1;
    map["SUBD"] = 2;
    map["ADDD"] = 2;
    map["DIVD"] = 3;
    map["MULTD"] = 4;
    mapp[1] = "整数";
    mapp[2] = "加法";
    mapp[3] = "除法";
    mapp[4] = "乘法1";
    mapp[5] = "乘法2";
    m[1][1] = "整";
    m[2][1] = "数";
    m[3][1] = "  ";
    m[1][2] = "加";
    m[2][2] = "法";
    m[3][2] = "  ";
    m[1][3] = "除";
    m[2][3] = "法";
    m[3][3] = "  ";
    m[1][4] = "乘";
    m[2][4] = "法";
    m[3][4] = "1 ";
    m[1][5] = "乘";
    m[2][5] = "法";
    m[3][5] = "2 ";
}

// 读指令
inline void read()
{
    // 读入要执行的指令数
    scanf("%d", &t);
    // 去除多余换行符，防止读入空行
    char ch;
    while (1)
    {
        ch = getchar();
        if (ch == '\n')
            break;
    }

    for (int i = 1; i <= t; i++)
    {
        std::string s;
        getline(std::cin, s);
        instruction[i].name = s;

        // 解析指令，根据指令类型初始化指令结构体的字段
        if (s.substr(0, 2) == "LD")
        {
            instruction[i].op = "LD";
            instruction[i].cycle = 1;

            // 解析目的寄存器和源操作数
            sscanf(s.c_str(), "%*s %*s F%d,%*d(R%d)", &instruction[i].fi, &instruction[i].fj);
        }
        else if (s.substr(0, 2) == "MU")
        {
            instruction[i].op = "MULTD";
            instruction[i].cycle = 10;

            // 解析目的寄存器和源操作数
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
        else if (s.substr(0, 2) == "SU")
        {
            instruction[i].op = "SUBD";
            instruction[i].cycle = 2;

            // 解析目的寄存器和源操作数
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
        else if (s.substr(0, 2) == "DI")
        {
            instruction[i].op = "DIVD";
            instruction[i].cycle = 40;

            // 解析目的寄存器和源操作数
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
        else if (s.substr(0, 2) == "AD")
        {
            instruction[i].op = "ADDD";
            instruction[i].cycle = 2;

            // 解析目的寄存器和源操作数
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
    }
// 执行阶段
// 枚举执行队列
for (it = step[3].begin(); it != step[3].end(); ++it)
{
    int x = *it;
    // 根据指令类型进行相应的处理
    if (instruction[x].op == "LD")
    {
        // LD 指令执行完成后，将目的寄存器中的值更新为从内存中加载的值
        int cycle;
        instruction[x].execution = cycle;
        // 模拟从内存中加载数据的过程
        // 假设从内存地址 Mem[instruction[x].fj] 中加载数据到寄存器 instruction[x].fi
        reg[instruction[x].fi] = Mem[instruction[x].fj];
        // 将执行阶段完成的指令 x 放入写结果队列，并从执行队列删除
        now.push((st){x, 4});
    }
    else if (instruction[x].op == "ADDD" || instruction[x].op == "SUBD" || instruction[x].op == "DIVD" || instruction[x].op == "MULTD")
    {
        // 其他类型指令的执行逻辑
        // 省略部分代码，根据你的具体需求实现
    }
}
// 写结果阶段
// 枚举写结果队列
for (it = step[4].begin(); it != step[4].end(); ++it)
{
    int x = *it;
    // 根据指令类型进行相应的处理
    if (instruction[x].op == "LD")
    {
        // LD 指令不需要执行写结果阶段，直接跳过
        continue;
    }
    else if (instruction[x].op == "ADDD" || instruction[x].op == "SUBD" || instruction[x].op == "DIVD" || instruction[x].op == "MULTD")
    {
        // 计算指令的结果，并将结果写入结果寄存器或者存储器中
        // 这里需要根据你的具体需求来实现，例如将计算结果存入结果寄存器或者存储器
        // 然后更新 ROB 中的信息
        instruction[x].write = time;
        ROB[instruction[x].Dest].Value = instruction[x].Result; // 假设 ROB 的数据结构中有 Value 和 Dest 字段
        ROB[instruction[x].Dest].Ready = true;
        // 将写结果阶段完成的指令 x 放入确认队列，并从写结果队列删除
        now.push((st){x, 5});
    }
}
// 确认阶段
// 枚举确认队列
for (it = step[5].begin(); it != step[5].end(); ++it)
{
    int h = *it; // ROB 头部索引
    std::string instruction = ROB[h].Instruction;

    // 根据不同指令类型执行相应的确认操作
    if (instruction == "Branch")
    {
        // 分支指令的确认逻辑
        if (branchMispredicted()) // 如果分支预测错误
        {
            clearROBEntry(h);
            clearRegisterStatus(); // 清空寄存器状态
            fetchBranchDestination(); // 获取分支目标地址
        }
    }
    else if (instruction == "Store")
    {
        // 存储指令的确认逻辑
        Mem[ROB[h].Dest] = ROB[h].Value; // 假设 Mem 是存储器数组
    }
    else
    {
        // 其他类型指令的确认逻辑，将计算结果写入目标寄存器
        Regs[ROB[h].Dest] = ROB[h].Value; // 假设 Regs 是寄存器数组
    }

    ROB[h].Busy = false; // 释放 ROB 条目

    // 如果目标寄存器没有其它指令正在写入，则释放该寄存器
    if (RegStat[ROB[h].Dest].Reorder == h)
    {
        RegStat[ROB[h].Dest].Busy = false;
    }
}
// 分支预测错误检查函数
bool branchMispredicted()
{
    // 这里是分支预测错误的逻辑判断
    // 返回 true 表示分支预测错误，否则返回 false
    // 这里假设 mispredicted() 是一个用于判断分支预测是否错误的函数
    return mispredicted();
}

// 清空 ROB 条目函数
void clearROBEntry(int index)
{
    // 这里是清空 ROB 条目的逻辑
    // 清空指定索引的 ROB 条目
    ROB[index].Value = 0;
    ROB[index].Ready = false;
    // 继续清空其它字段...
}

// 清空寄存器状态函数
void clearRegisterStatus()
{
    // 这里是清空寄存器状态的逻辑
    // 清空所有寄存器的状态信息，如忙闲状态等
    for (int i = 0; i < NumRegisters; ++i)
    {
        RegStat[i].Busy = false;
        // 继续清空其它字段...
    }
}

// 获取分支目标地址函数
void fetchBranchDestination()
{
    // 这里是获取分支目标地址的逻辑
    // 获取分支预测错误时的目标地址，并将 PC 设置为该地址
    PC = mispredictedBranchTarget();
}
// 结果写回阶段（确认阶段）
void writeBackStage()
{
    // 如果要处理的指令位于 ROB 的头部且指令已准备就绪
    if (ROBHeadReady())
    {
        // 获取目标寄存器编号
        int destReg = ROBHeadDestination();

        // 分支指令特殊处理
        if (ROBHeadInstruction() == Branch)
        {
            // 检查是否分支预测错误
            if (branchMispredicted())
            {
                // 清空 ROB 条目和寄存器状态
                clearROBEntry(ROBHeadIndex());
                clearRegisterStatus();

                // 获取分支目标地址并设置 PC
                fetchBranchDestination();
            }
        }
        else if (ROBHeadInstruction() == Store)
        {
            // 存储指令写回内存
            Mem[ROBHeadDestination()] = ROBHeadValue();
        }
        else
        {
            // 将结果写入目标寄存器
            Regs[destReg] = ROBHeadValue();

            // 标记 ROB 条目为空闲
            ROB[ROBHeadIndex()].Busy = false;

            // 如果目标寄存器状态等于当前 ROB 头部索引，则释放目标寄存器
            if (RegStat[destReg].Reorder == ROBHeadIndex())
            {
                RegStat[destReg].Busy = false;
            }
        }
    }
}
// 主函数
int main()
{
    // 初始化
    init();

    // 读取指令
    readInstructions();

    // 执行 Tomasulo 算法
    executeTomasulo();

    return 0;
}

