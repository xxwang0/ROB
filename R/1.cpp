#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

// 假设一些数据结构和全局变量
struct ROBEntry {
    string instruction;
    int dest;
    float value;
    bool ready;
    bool busy;
    int address; // 用于存储指令的地址
};

struct RSEntry {
    float Vj, Vk;
    int Qj, Qk;
    bool busy;
    int Dest;
    int A;
};

struct RegisterStatus {
    bool busy;
    int reorder;
};

// 示例数据结构定义
vector<ROBEntry> ROB;
vector<RSEntry> RS;
vector<float> Regs;
vector<RegisterStatus> RegStat;
vector<float> Mem;
queue<int> q; // 队列存储待处理的指令
queue<pair<int, int> > now; // 执行队列
string op;
int rs, rt, imm;

int find_free_RS() {
    for (int i = 0; i < RS.size(); ++i) {
        if (!RS[i].busy) return i;
    }
    return -1;
}

int find_free_ROB() {
    for (int i = 0; i < ROB.size(); ++i) {
        if (!ROB[i].busy) return i;
    }
    return -1;
}

void issue(int time) {
    if (!q.empty()) {
        int x = q.front();    
        int r = find_free_RS(); // 找到一个空闲的保留站
        int b = find_free_ROB(); // 找到一个空闲的ROB项
        if (r == -1 || b == -1) {
            cout << "No free reservation station or ROB entry available.\n";
            return;
        }
        // 检查源寄存器的状态
        if (RegStat[rs].busy) {
            int h = RegStat[rs].reorder;
            if (ROB[h].ready) {
                RS[r].Vj = ROB[h].value; // 如果ROB头指令完成则取数
                RS[r].Qj = 0;
            } else {
                RS[r].Qj = h; // 否则，等待指令
            }
        } 
        else {
            RS[r].Vj = Regs[rs]; // 若寄存器空闲，直接取数
            RS[r].Qj = 0;
        }

        RS[r].busy = true;
        RS[r].Dest = b;
        ROB[b].instruction = op;
        ROB[b].dest = rt;
        ROB[b].ready = false;

        // 如果是浮点运算或存储操作
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV" || op == "STORE") {
            if (RegStat[rt].busy) {
                int h = RegStat[rt].reorder;
                if (ROB[h].ready) {
                    RS[r].Vk = ROB[h].value; // 如果ROB头指令完成则取数
                    RS[r].Qk = 0;
                } else {
                    RS[r].Qk = h; // 否则，等待指令
                }
            } else {
                RS[r].Vk = Regs[rt]; // 若寄存器空闲，直接取数
                RS[r].Qk = 0;
            }
        }

        // 对于浮点操作
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
            RegStat[rt].reorder = b;
            RegStat[rt].busy = true;
            ROB[b].dest = rt;
        }

        // 对于store操作
        if (op == "STORE") {
            RS[r].A = imm;
        }

        // 对于load操作
        if (op == "LOAD") {
            RS[r].A = imm;
            RegStat[rt].reorder = b;
            RegStat[rt].busy = true;
            ROB[b].dest = rt;
        }

        // 将发射阶段完成的 x 放入执行队列
        now.push(make_pair(x, 2));
        q.pop(); // 从任务队列中移除已发射的任务
    }
}

void execute(int time) {
    while (!now.empty()) {
        auto task = now.front();
        int x = task.first;
        now.pop();

        int r = x; // 假设 x 是保留站索引
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
            if (RS[r].Qj == 0 && RS[r].Qk == 0) {
                // 计算结果
                float result = RS[r].Vj + RS[r].Vk; // 假设为加法操作
                // 将结果存储到 ROB 中
                ROB[RS[r].Dest].value = result;
                ROB[RS[r].Dest].ready = true;
            }
        } else if (op == "LOAD") {
            if (RS[r].Qj == 0) {
                RS[r].A = RS[r].Vj + RS[r].A;
                ROB[RS[r].Dest].value = Mem[RS[r].A];
                ROB[RS[r].Dest].ready = true;
            }
        } else if (op == "STORE") {
            if (RS[r].Qj == 0) {
                ROB[RS[r].Dest].address = RS[r].Vj + RS[r].A;
                // 其他存储相关操作
            }
        }
    }
}

// 假设执行结果写回阶段的函数
void execute_write_result(int r, float result) {
    int b = RS[r].Dest; // 取得目标 ROB 条目索引

    RS[r].busy = false; // 将当前保留站置为非忙状态

    // 更新所有依赖于当前指令结果的保留站的状态
    for (int x = 0; x < RS.size(); ++x) {
        if (RS[x].Qj == b) {
            RS[x].Vj = result; // 将结果写入 Vj
            RS[x].Qj = 0; // 清除 Qj
        }
        if (RS[x].Qk == b) {
            RS[x].Vk = result; // 将结果写入 Vk
            RS[x].Qk = 0; // 清除 Qk
        }
    }

    // 将结果写入 ROB
    ROB[b].value = result;
    ROB[b].ready = true; // 标记为就绪状态
}

// 假设提交阶段的函数实现
void commit() {
    // 确认要处理的指令位于 ROB 的头部且已就绪
    if (!ROB.empty() && ROB[0].ready) {
        int h = 0; // ROB 头部索引
        int d = ROB[h].dest; // 目标寄存器索引

        if (ROB[h].instruction == "STORE") {
            // 处理存储指令
            Mem[ROB[h].address] = ROB[h].value; // 存储到内存
        } else {
            // 处理其他指令，将结果写入目标寄存器
            Regs[d] = ROB[h].value;
        }

        // 释放 ROB 条目
        ROB[h].busy = false;

        // 如果没有其他指令正在写入目标寄存器，则释放目标寄存器
        if (RegStat[d].reorder == h) {
            RegStat[d].busy = false;
        }

        // 移除 ROB 头部条目
        ROB.erase(ROB.begin());
    }
}

int main() {
    // 初始化数据结构和队列等
    // 假设 RS、ROB、Regs、RegStat、Mem 的大小初始化
    RS.resize(10);
    ROB.resize(10);
    Regs.resize(32);
    RegStat.resize(32);
    Mem.resize(1024);

    // 示例任务入队
    q.push(0);
    q.push(1);
    q.push(2);

    // 设置全局变量示例
    op = "ADD";
    rs = 1;
    rt = 2;
    imm = 10;

    // 示例时钟周期模拟
    int time = 0;
    while (!q.empty() || !now.empty()) {
        issue(time);
        execute(time);
        commit();
        time++;
    }

    return 0;
}

