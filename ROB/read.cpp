#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>

using namespace std;

struct ReservationStation {
    bool Busy;   // 是否被占用
    string Op;   // 操作类型，如 "LOAD", "ADD", "MULT"
    int Vj, Vk;  // 操作数值
    int Qj, Qk;  // 操作数状态
    int Dest;    // 结果寄存器
    int A;       // 立即数或偏移量
};

struct ROB_Entry {
    bool Busy;       // 是否被占用
    string Op;       // 操作类型
    int Dest;        // 目标寄存器
    int Value;       // 结果值
    bool Ready;      // 是否就绪
    int Address;     // 内存地址或立即数
};

struct instr  //存放指令的各个细节 
{
	std::string op;
	int fi,fj,fk,time,A;
	int issue,exeution,write,commit;
	std::string name;
} instruction[101];


// vector<instruction> instructions;
vector<ReservationStation> LoadRS(2);   // Load保留站，假设有2个
vector<ReservationStation> AddRS(2);    // 加法保留站，假设有2个
vector<ReservationStation> MultRS(2);   // 乘法保留站，假设有2个
vector<ROB_Entry> ROB(6);               // 假设ROB有6个条目
vector<int> RegisterStatus(32, -1);     // 寄存器状态表，初始化为-1表示未被占用

queue<int> InstructionQueue;  // 指令队列

int cycle = 1;  // 当前周期数
int current_instruction = 0;  // 当前处理的指令索引

int main(){
	int t; 
scanf("%d", &t);

    // 去除多余换行符，防止读入空行
    char ch;
    while (1) {
        ch = getchar();
        if (ch == '\n') break;
    }

    // 逐条读入指令
    for (int i = 1; i <= t; i++) {
        std::string s;
        getline(std::cin, s); // 读取一行指令字符串

        instruction[i].name = s; // 将原始指令字符串存储在 instruction 结构体中的 name 字段

        // 根据指令的首字母判断指令类型并解析
        if (s[0] == 'L'||s[0] == 'S') {
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
                	cnt=j;
                	m=s[cnt + 1] - '0';
                	instruction[i].fj = m;
                    break;
                }
            }
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m*= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].fj = m;
            }
            
        } 
		else {
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
                instruction[i].time = 20; // DIVD 指令执行时间设为 40 个周期
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
            int m= s[cnt + 1] - '0';
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
            m= s[cnt + 1] - '0';
            instruction[i].fk = m; // 提取源操作数寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].fk = m;
            }
        }
    }
    for (int i = 1; i <= t; ++i) {
        cout << instruction[i].op << " " <<instruction[i].fi << " " <<instruction[i].fj << " " <<instruction[i].fk << " " <<instruction[i].A << " " << instruction[i].name << endl;
    }
return 0;
}


