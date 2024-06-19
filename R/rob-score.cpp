#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <vector>
using namespace std;

const int MAX_INSTR = 100;

// 功能单元类型
enum FuncUnitType {
    INT,    // 整数运算
    ADD,    // 加法
    DIV,    // 除法
    MULT1,  // 乘法1
    MULT2   // 乘法2
};

// 指令结构体
struct Instruction {
    string name;    // 指令名称
    string op;      // 操作类型
    int fi, fj, fk; // 操作数寄存器编号
    int issue, read, execution, write; // 时间戳
    int rob_entry;  // ROB条目编号
    bool rob_done;  // 标记指令是否完成执行

    // 构造函数初始化
    Instruction() : issue(0), read(0), execution(0), write(0), rob_entry(-1), rob_done(false) {}
};

// 预约站（Reservation Station）结构体
struct ReservationStation {
    bool busy;           // 是否忙碌
    FuncUnitType type;   // 功能单元类型
    int op;              // 操作类型编号
    int vj, vk;          // 操作数值
    int qj, qk;          // 操作数就绪状态（0表示就绪）
    int dest;            // 目的寄存器
    int instruction_idx; // 指令在指令数组中的索引

    ReservationStation() : busy(false), type(INT), op(-1), vj(0), vk(0), qj(0), qk(0), dest(0), instruction_idx(-1) {}
};

// ROB（Reorder Buffer）结构体
struct ReorderBuffer {
    int head; // ROB头指针
    int tail; // ROB尾指针
    vector<int> entries; // ROB条目

    ReorderBuffer() : head(0), tail(0) {}
};

// 全局变量
int t; // 指令数量
Instruction instructions[MAX_INSTR + 1]; // 指令数组
ReservationStation reservationStations[5]; // 预约站数组（5个功能单元）
ReorderBuffer rob; // ROB

// 指令操作类型映射
struct OpTypeMapInitializer {
    map<string, FuncUnitType> operator()() const {
        map<string, FuncUnitType> temp;
        temp["LD"] = INT;
        temp["SUBD"] = ADD;
        temp["ADDD"] = ADD;
        temp["DIVD"] = DIV;
        temp["MULTD"] = MULT1;
        return temp;
    }
};
static OpTypeMapInitializer opTypeMapInitializer;
map<string, FuncUnitType> opTypeMap = opTypeMapInitializer();

// 操作类型字符串映射
map<FuncUnitType, string> funcUnitTypeStr;

void initializeFuncUnitTypeStr() {
    funcUnitTypeStr[INT] = "整数";
    funcUnitTypeStr[ADD] = "加法";
    funcUnitTypeStr[DIV] = "除法";
    funcUnitTypeStr[MULT1] = "乘法1";
    funcUnitTypeStr[MULT2] = "乘法2";
}

// 初始化函数
void init() {
    rob.entries.resize(MAX_INSTR + 1, -1); // 初始化ROB条目
    initializeFuncUnitTypeStr(); // 初始化功能部件类型字符串映射
}

// 读入指令函数
void read() {
    scanf("%d\n", &t); // 读入指令数量

    for (int i = 1; i <= t; ++i) {
        string s;
        getline(cin, s); // 读入一行指令

        instructions[i].name = s; // 记录指令名称

        // 解析指令类型和操作数
        if (s[0] == 'L') {
            instructions[i].op = "LD";
            sscanf(s.c_str(), "LD F%d,%*d(R%d)", &instructions[i].fi, &instructions[i].fj);
        } else {
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instructions[i].fi, &instructions[i].fj, &instructions[i].fk);
            if (s[0] == 'S') instructions[i].op = "SUBD";
            else if (s[0] == 'A') instructions[i].op = "ADDD";
            else if (s[0] == 'D') instructions[i].op = "DIVD";
            else if (s[0] == 'M') instructions[i].op = "MULTD";
        }
    }
}

// 打印状态函数
void print(int cycle) {
    printf("                                                周期%d:\n", cycle);
    printf("指令状态:\n");
    printf("指令名        |Issue|Read operand|Execution completed|Write Result|\n");
    for (int i = 1; i <= t; ++i) {
        printf("%-14s|", instructions[i].name.c_str());
        if (instructions[i].issue != 0) printf("%5d|", instructions[i].issue);
        else printf("     |");
        if (instructions[i].read != 0) printf("%12d|", instructions[i].read);
        else printf("            |");
        if (instructions[i].execution != 0) printf("%19d|", instructions[i].execution);
        else printf("                   |");
        if (instructions[i].write != 0) printf("%12d|", instructions[i].write);
        else printf("            |");
        printf("\n");
    }
    printf("\n");
    printf("功能部件状态:\n");
    printf("功能部件名|Busy |Op   |Fi |Fj |Fk |Qj   |Qk   |Rj |Rk |\n");
    for (int i = 0; i < 5; ++i) {
        printf("%-10s|", funcUnitTypeStr[reservationStations[i].type].c_str());
        if (reservationStations[i].busy) printf("true |");
        else printf("false|     |   |   |   |     |     |   |   |\n");
    }
    printf("\n");
    printf("ROB状态:\n");
    printf("ROB条目|指令名        |Issue|Read operand|Execution completed|Write Result|\n");
    for (int i = rob.head; i < rob.tail; ++i) {
        int idx = rob.entries[i];
        printf("%-7d|", i);
        if (idx != -1) {
            printf("%-14s|", instructions[idx].name.c_str());
            if (instructions[idx].issue != 0) printf("%5d|", instructions[idx].issue);
            else printf("     |");
            if (instructions[idx].read != 0) printf("%12d|", instructions[idx].read);
            else printf("            |");
            if (instructions[idx].execution != 0) printf("%19d|", instructions[idx].execution);
            else printf("                   |");
            if (instructions[idx].write != 0) printf("%12d|", instructions[idx].write);
            else printf("            |");
        }
        printf("\n");
    }
    printf("\n");
}

// Tomasulo算法流程
void tomasulo() {
    queue<int> instr_queue; // 指令队列
    int cycle = 0; // 周期计数器

    for (int i = 1; i <= t; ++i) instr_queue.push(i); // 将所有指令入队

    while (!instr_queue.empty() || rob.head < rob.tail) {
        cycle++;

        // 发射阶段
        for (int i = 0; i < 5; ++i) {
            if (!reservationStations[i].busy && !instr_queue.empty()) {
                int idx = instr_queue.front();
                instr_queue.pop();
                instructions[idx].issue = cycle;
                reservationStations[i].busy = true;
                reservationStations[i].type = opTypeMap[instructions[idx].op];
                reservationStations[i].op = i;
                reservationStations[i].vj = instructions[idx].fj;
                reservationStations[i].vk = instructions[idx].fk;
                reservationStations[i].qj = reservationStations[i].qk = 0;
                reservationStations[i].dest = instructions[idx].fi;
                reservationStations[i].instruction_idx = idx;
            }
        }

        // 执行阶段
        for (int i = 0; i < 5; ++i) {
            if (reservationStations[i].busy) {
                int idx = reservationStations[i].instruction_idx;
                if (reservationStations[i].qj == 0 && reservationStations[i].qk == 0) {
                    reservationStations[i].busy = false;
                    instructions[idx].execution = cycle;
                    instructions[idx].write = cycle + 1;
                    instructions[idx].rob_done = true;
                }
            }
        }

        // 退役阶段
        while (rob.head < rob.tail && instructions[rob.entries[rob.head]].rob_done) rob.head++;

        // 打印状态
        print(cycle);
    }
}

// 主函数
int main() {
    init(); // 初始化

    read(); // 读入指令

    tomasulo(); // 运行Tomasulo算法

    return 0;
}

