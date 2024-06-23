#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

struct ReservationStation {
    bool Busy;   // 是否被占用
    string op;   // 操作类型
    int Vj, Vk;  // 操作数值
    int Qj, Qk;  // 操作数状态
    int Dest;    // 结果寄存器
    int A;       // 立即数或偏移量
};

struct ROB_Entry {
    bool Busy;       // 是否被占用
    string op;       // 操作类型
    int Dest;        // 目标寄存器
    int Value;       // 结果值
    bool Ready;      // 是否就绪
    int Address;     // 内存地址或立即数
};

struct instr {  // 存放指令的各个细节
    string op;
    int fi, fj, fk, time, A;
    int issue, execute, write, commit;
    string name;
} instruction[101];

// vector<instruction> instructions;
vector<ReservationStation> LoadRS(2);   // Load保留站，假设有2个
vector<ReservationStation> AddRS(2);    // 加法保留站，假设有2个
vector<ReservationStation> MultRS(2);   // 乘法保留站，假设有2个
vector<ROB_Entry> ROB(6);               // 假设ROB有6个条目
vector<int> RegisterStatus(32, -1);     // 寄存器状态表，初始化为-1表示未被占用

queue<int> q;  // 指令队列

int cycle = 0;  // 当前周期数
int time = 1;
int t; // 指令数量

void read() {
    scanf("%d", &t);

    // 去除多余换行符，防止读入空行
    char ch;
    while (1) {
        ch = getchar();
        if (ch == '\n') break;
    }

    // 逐条读入指令
    for (int i = 1; i <= t; i++) {
        string s;
        getline(cin, s); // 读取一行指令字符串

        instruction[i].name = s; // 将原始指令字符串存储在 instruction 结构体中的 name 字段

        // 根据指令的首字母判断指令类型并解析
        if (s[0] == 'L' || s[0] == 'S') {
            // 处理 LD 指令
            instruction[i].op = "LD"; // 设置指令操作类型为 LD
            instruction[i].time = 2; // LD 指令执行周期为 2 个周期

            // 解析 fi：目标寄存器编号
            int cnt = 0;
            for (int j = 2; j < s.length(); j++) {
                if (s[j] != ' ') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fi = s[cnt + 1] - '0'; // 目的寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fi *= 10; // 如果目标寄存器编号是两位数，则进行倍增
                instruction[i].fi += s[cnt + 2] - '0'; // 加上个位数
            }
            for (int j = 2; j < s.length(); j++) {
                if (s[j] == ',') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].A = s[cnt + 1] - '0'; // 目的寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].A *= 10; // 如果目标寄存器编号是两位数，则进行倍增
                instruction[i].A += s[cnt + 2] - '0'; // 加上个位数
            }

            // 解析 fj：源操作数寄存器编号
            int m;
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'R') {
                    cnt = j;
                    m = s[cnt + 1] - '0';
                    instruction[i].fj = m;
                    break;
                }
            }
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].fj = m;
            }

        } else {
            // 处理其他类型的指令（MULTD, SUBD, ADDD, DIVD）
            int cnt = 0;
            if (s[0] == 'M') {
                instruction[i].op = "MULTD"; // 设置指令操作类型为 MULTD
                instruction[i].time = 10; // MULTD 指令执行时间设为 10 个周期
            } else if (s[0] == 'S') {
                instruction[i].op = "SUBD"; // 设置指令操作类型为 SUBD
                instruction[i].time = 2; // SUBD 指令执行时间设为 2 个周期
            } else if (s[0] == 'A') {
                instruction[i].op = "ADDD"; // 设置指令操作类型为 ADDD
                instruction[i].time = 2; // ADDD 指令执行时间设为 2 个周期
            } else if (s[0] == 'D') {
                instruction[i].op = "DIVD"; // 设置指令操作类型为 DIVD
                instruction[i].time = 20; // DIVD 指令执行时间设为 20 个周期
            }

            // 解析 fi：目标寄存器编号
            for (int j = 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fi = s[cnt + 1] - '0'; // 提取目标寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fi *= 10; // 如果目标寄存器编号是两位数，则进行倍增
                instruction[i].fi += s[cnt + 2] - '0'; // 加上个位数
            }

            // 解析 fj：源操作数寄存器编号
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            int m = s[cnt + 1] - '0';
            instruction[i].fj = m; // 提取源操作数寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].fj = m;
            }

            // 解析 Vk：第二个源操作数寄存器编号
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            m = s[cnt + 1] - '0';
            instruction[i].fk = m; // 提取源操作数寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].fk = m;
            }
        }
    }
}

void Issue() {
    for (int i = 1; i <= t; i++) q.push(i);
    cycle++;
    while (!q.empty()) {
        int x = q.front();
        if (instruction[x].op == "LD") {  // 如果操作类型是“LD”（加载）
            for (int i = 0; i < LoadRS.size(); ++i) {  // 遍历加载保留站
                if (!LoadRS[i].Busy) {  // 如果该保留站未被占用
                    LoadRS[i].Busy = true;  // 标记保留站为占用状态
                    LoadRS[i].op = instruction[x].op;  // 设置操作类型
                    LoadRS[i].Dest = instruction[x].fi;  // 设置目标寄存器
                    LoadRS[i].A = instruction[x].A;  // 设置立即数或偏移量

                    // 如果源操作数状态为可用（即寄存器状态表中对应寄存器状态为-1），直接读取操作数值
                    if (RegisterStatus[instruction[x].fj] == -1) {
                        LoadRS[i].Vj = instruction[x].fj;
                        LoadRS[i].Qj = -1;
                    } else {  // 否则，设置保留站中操作数状态为寄存器状态表中的值
                        LoadRS[i].Qj = RegisterStatus[instruction[x].fj];
                    }

                    // 标记目标寄存器为占用状态
                    RegisterStatus[instruction[x].fi] = i;

                    // 设置指令发射周期
                    instruction[x].issue = cycle;

                    // 将指令从队列中弹出
                    q.pop();

                    // 跳出循环，继续处理下一条指令
                    break;
                }
            }
        } else {  // 如果操作类型不是“LD”（即为算术运算指令）
            if (instruction[x].op == "ADDD" || instruction[x].op == "SUBD") {  // 如果是加法或减法指令
                for (int i = 0; i < AddRS.size(); ++i) {  // 遍历加法保留站
                    if (!AddRS[i].Busy) {  // 如果该保留站未被占用
                        AddRS[i].Busy = true;  // 标记保留站为占用状态
                        AddRS[i].op = instruction[x].op;  // 设置操作类型
                        AddRS[i].Dest = instruction[x].fi;  // 设置目标寄存器

                        // 如果源操作数状态为可用，直接读取操作数值
                        if (RegisterStatus[instruction[x].fj] == -1) {
                            AddRS[i].Vj = instruction[x].fj;
                            AddRS[i].Qj = -1;
                        } else {
                            AddRS[i].Qj = RegisterStatus[instruction[x].fj];
                        }

                        // 如果源操作数状态为可用，直接读取操作数值
                        if (RegisterStatus[instruction[x].fk] == -1) {
                            AddRS[i].Vk = instruction[x].fk;
                            AddRS[i].Qk = -1;
                        } else {
                            AddRS[i].Qk = RegisterStatus[instruction[x].fk];
                        }

                        // 标记目标寄存器为占用状态
                        RegisterStatus[instruction[x].fi] = i;

                        // 设置指令发射周期
                        instruction[x].issue = cycle;

                        // 将指令从队列中弹出
                        q.pop();

                        // 跳出循环，继续处理下一条指令
                        break;
                    }
                }
            } else {  // 如果是乘法或除法指令
                for (int i = 0; i < MultRS.size(); ++i) {  // 遍历乘法保留站
                    if (!MultRS[i].Busy) {  // 如果该保留站未被占用
                        MultRS[i].Busy = true;  // 标记保留站为占用状态
                        MultRS[i].op = instruction[x].op;  // 设置操作类型
                        MultRS[i].Dest = instruction[x].fi;  // 设置目标寄存器

                        // 如果源操作数状态为可用，直接读取操作数值
                        if (RegisterStatus[instruction[x].fj] == -1) {
                            MultRS[i].Vj = instruction[x].fj;
                            MultRS[i].Qj = -1;
                        } else {
                            MultRS[i].Qj = RegisterStatus[instruction[x].fj];
                        }

                        // 如果源操作数状态为可用，直接读取操作数值
                        if (RegisterStatus[instruction[x].fk] == -1) {
                            MultRS[i].Vk = instruction[x].fk;
                            MultRS[i].Qk = -1;
                        } else {
                            MultRS[i].Qk = RegisterStatus[instruction[x].fk];
                        }

                        // 标记目标寄存器为占用状态
                        RegisterStatus[instruction[x].fi] = i;

                        // 设置指令发射周期
                        instruction[x].issue = cycle;

                        // 将指令从队列中弹出
                        q.pop();

                        // 跳出循环，继续处理下一条指令
                        break;
                    }
                }
            }
        }
    }
}

void Execute() {
    // 执行阶段，假设每个操作为一个周期完成

    // 遍历加载保留站
    for (int i = 0; i < LoadRS.size(); ++i) {
        if (LoadRS[i].Busy) {  // 如果该加载保留站被占用
            // 模拟LD操作，假设执行完毕
            ROB[LoadRS[i].Dest].Value = LoadRS[i].Vj + LoadRS[i].A;  // 计算结果值并写入ROB
            ROB[LoadRS[i].Dest].Ready = true;  // 标记ROB条目为就绪状态
            instruction[LoadRS[i].Dest].execute = cycle;  // 记录指令执行的周期数

            LoadRS[i].Busy = false;  // 清除加载保留站状态
            
        }
    }

    // 遍历加法保留站
    for (int i = 0; i < AddRS.size(); ++i) {
        if (AddRS[i].Busy) {  // 如果该加法保留站被占用
            // 模拟ADD操作，假设执行完毕
            ROB[AddRS[i].Dest].Value = AddRS[i].Vj + AddRS[i].Vk;  // 计算结果值并写入ROB
            ROB[AddRS[i].Dest].Ready = true;  // 标记ROB条目为就绪状态
            instruction[AddRS[i].Dest].execute = cycle;  // 记录指令执行的周期数

            AddRS[i].Busy = false;  // 清除加法保留站状态
        }
       
    }

    // 遍历乘法保留站
    for (int i = 0; i < MultRS.size(); ++i) {
        if (MultRS[i].Busy) {  // 如果该乘法保留站被占用
            // 模拟MULT操作，假设执行完毕
            ROB[MultRS[i].Dest].Value = MultRS[i].Vj * MultRS[i].Vk;  // 计算结果值并写入ROB
            ROB[MultRS[i].Dest].Ready = true;  // 标记ROB条目为就绪状态
            instruction[MultRS[i].Dest].execute = cycle;  // 记录指令执行的周期数

            MultRS[i].Busy = false;  // 清除乘法保留站状态
        }
        
    }

    cycle+=2;  // 周期数加1，进入下一个周期
}


void Write() {
    // 写结果阶段，假设写回为一个周期完成
    for (int i = 0; i < ROB.size(); ++i) {
        if (ROB[i].Busy && ROB[i].Ready) {  // 如果ROB条目被占用且就绪
            int dest_reg = ROB[i].Dest;
            RegisterStatus[dest_reg] = ROB[i].Value;  // 将ROB中的结果值写入目标寄存器
            ROB[i].Busy = false;  // 标记ROB条目为未占用
            instruction[i].write = cycle;  // 记录指令写回的周期数
            
        }
    }
    cycle++;
}


void Commit() {
    int i = 0;
    bool committed = false; // 标记是否已经确认了一条指令

    while (i < ROB.size() && !committed) {
        if (ROB[i].Ready && ROB[i].Busy) { // 检查ROB条目是否就绪且被占用
            int d = ROB[i].Dest;
            RegisterStatus[d] = ROB[i].Value; // 将ROB中的结果写入目标寄存器
            ROB[i].Busy = false; // 标记ROB条目为未占用
            ROB[i].Ready = false; // 标记ROB条目为未就绪

            // 从ROB中删除该指令
            ROB.erase(ROB.begin() + i);

            instruction[i].commit = cycle; // 记录指令提交的周期数
            committed = true; // 标记已确认了一条指令
        } else {
            i++; // 如果当前ROB条目不满足条件，检查下一个条目
        }
    }
    cycle++;
}


void Print() {
    // 输出指令状态表
    	printf("                                                周期%d:\n",&cycle); 
    printf("指令名        |流出|执行|写结果|确认|\n");
    for (int i = 1; i <= t; i++) {
        cout << instruction[i].name; // 对齐指令名
        for (int j = 1; j <= 14 - instruction[i].name.length(); j++) printf(" ");
        printf("|");
        // 输出流出时间
        if (instruction[i].issue != 0) printf("%4d|", instruction[i].issue);
        else printf("    |");
        // 输出执行周期
        if (instruction[i].execute != 0) printf("%4d|", instruction[i].execute);
        else printf("    |");
        // 输出写结果周期
        if (instruction[i].write != 0) printf("%6d|", instruction[i].write);
        else printf("      |");
        // 输出确认周期
        if (instruction[i].commit != 0) printf("%4d|", instruction[i].commit);
        else printf("    |");
        printf("\n");
    }
    printf("\n");

    // 输出保留站状态表
    printf("保留站:\n");
    printf(" 名称|Busy |   Op|Vj |Vk |Qj |Qk |Dest|  A|\n");
    for (int i = 0; i < 2; i++) {
        printf("load%d|", i + 1);
        if (LoadRS[i].Busy) printf("true |");
        else printf("false|");
        printf(" %4s|", LoadRS[i].op.c_str());
        printf(" %2d|", LoadRS[i].Vj);
        printf(" %2d|", LoadRS[i].Vk);
        printf(" %2d|", LoadRS[i].Qj);
        printf(" %2d|", LoadRS[i].Qk);
        printf(" %3d|", LoadRS[i].Dest);
        printf(" %2d|\n", LoadRS[i].A);
    }
    for (int i = 0; i < 2; i++) {
        printf(" add%d|", i + 1);
        if (AddRS[i].Busy) printf("true |");
        else printf("false|");
        printf(" %4s|", AddRS[i].op.c_str());
        printf(" %2d|", AddRS[i].Vj);
        printf(" %2d|", AddRS[i].Vk);
        printf(" %2d|", AddRS[i].Qj);
        printf(" %2d|", AddRS[i].Qk);
        printf(" %3d|", AddRS[i].Dest);
        printf(" %2d|\n", AddRS[i].A);
    }
    for (int i = 0; i < 2; i++) {
        printf("mult%d|", i + 1);
        if (MultRS[i].Busy) printf("true |");
        else printf("false|");
        printf(" %4s|", MultRS[i].op.c_str());
        printf(" %2d|", MultRS[i].Vj);
        printf(" %2d|", MultRS[i].Vk);
        printf(" %2d|", MultRS[i].Qj);
        printf(" %2d|", MultRS[i].Qk);
        printf(" %3d|", MultRS[i].Dest);
        printf(" %2d|\n", MultRS[i].A);
    }
    printf("\n");

    // 输出ROB状态表
    printf("ROB状态表:\n");
    printf("序号|Busy |Op   |Dest | 值 |Ready |\n");
    for (int i = 0; i < ROB.size(); i++) {
        printf(" %2d |", i + 1);
        if (ROB[i].Busy) printf("true |");
        else printf("false|");
        printf(" %4s|", ROB[i].op.c_str());
        printf(" %4d|", ROB[i].Dest);
        printf(" %3d|", ROB[i].Value);
        if (ROB[i].Ready) printf(" true |\n");
        else printf(" false|\n");
    }
    printf("\n");

    // 输出结果寄存器表
    printf("结果寄存器:\n");
    printf(" 字段 |F0|F1|F2|F3|F4|F5|F6|F7|F8|F9\n");
    printf("  busy|");
    for (int i = 0; i < 9; i++) {
        if (RegisterStatus[i] != -1) printf("%2d|", i);
        else printf("  |");
    }
    printf("\n  rob |");
    for (int i = 0; i < 9; i++) {
        if (RegisterStatus[i] != -1) printf("%2d|", RegisterStatus[i]);
        else printf("  |");
    }
    printf("\n value|");
    for (int i = 0; i < 9; i++) {
        if (RegisterStatus[i] != -1) printf("%2d|", RegisterStatus[i]);
        else printf("  |");
    }
    printf("\n");
}


int main(){
	read();
	Issue();
	Execute();
	Write();
	Commit();
	Print();
	return 0;
}




