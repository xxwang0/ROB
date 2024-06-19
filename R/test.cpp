#include <cstdio>       // C标准输入输出库
#include <algorithm>    // 标准算法库
#include <string>       // 字符串库
#include <iostream>     // 标准输入输出流库
#include <queue>        // 队列容器库
#include <set>          // 集合容器库
#include <map>          // 映射容器库
#include <windows.h>    // Windows平台特定库
#include <cstdlib>      // C标准库

using namespace std;    // 使用标准库的命名空间

int t;
 
queue<int> q;//指令队列（发射队列） 
struct st
{
	int n,step;
};

struct Regist
{
	int busy,value;
} regist[101];

struct instr  //存放指令的各个细节 
{
	string op;
	int Vj,Vk,time,device,Qj,Qk,Dest,A;
	int issue,exeution,write,commit;
	string name;
} instruction[101];
struct ROB
{
	int busy;
    string op; // 指令名称
    int Dest;
    int value;
    int ready; 
    int Address;
}rob[101];

queue<int> reg_q[32];//存放需要该寄存器结果的指令 

int reg[32];//存放写该寄存器的指令 
int device[8];//存放占用该功能部件的指令 
set<int> step[6];//存放各个阶段的指令队列 
set<int>::iterator it,ite;//迭代器，用来枚举 
map<string,int> map;//用来做映射，将数字与具体指代指令联系 
map<int,string> mapp;//用来做映射，将数字与具体指代指令联系
string m[4][32];//用来做映射，将数字与具体指代指令联系

//初始化

inline void init()
{
	map["LD"] = 1;map["SUBD"] = 2;map["ADDD"] = 2;map["DIVD"] = 3;map["MULTD"] = 4;//用来做映射，将数字与具体指代指令联系 ,操作码 
	mapp[1] = "Load1";mapp[2] = "Load2";mapp[3] = "Add1";mapp[4] = "Add2";mapp[5] = "Mult1"; mapp[6] = "Mult2";//保留站 
//	m[1][1] = "整";m[2][1] = "数";m[3][1] = "  ";
//	m[1][2] = "加";m[2][2] = "法";m[3][2] = "  "; 
//	m[1][3] = "除";m[2][3] = "法";m[3][3] = "  "; 
//	m[1][4] = "乘";m[2][4] = "法";m[3][4] = "1 "; 
//	m[1][5] = "乘";m[2][5] = "法";m[3][5] = "2 ";
}

// 读取指令并解析
inline void read() {
    // 读入要执行的指令数
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
        if (s[0] == 'L') {
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

            // 解析 fj：源操作数寄存器编号
            int m;
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'R') {
                	cnt=j;
                	m=s[cnt + 1] - '0'
                	instruction[i].Vj = R[m];
                    break;
                }
            }
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m*= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].Vj = R[m];
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
            m= s[cnt + 1] - '0';
            instruction[i].Vj = R[m] // 提取源操作数寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].Vj = R[m]
            }

            // 解析 Vk：第二个源操作数寄存器编号
           for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            m= s[cnt + 1] - '0';
            instruction[i].Vk = R[m] // 提取源操作数寄存器编号的十位数
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // 如果源操作数寄存器编号是两位数，则进行倍增
                m += s[cnt + 2] - '0'; // 加上个位数
                instruction[i].Vk = R[m]
            }
        }
    }
}

//输出结果 
inline void print(int x)
{
	printf("                                                周期%d:\n",x); 
	//输出指令状态表 
	printf("指令状态表:\n");
	printf("指令名        |流出|执行|写结果|确认|\n");
	for(int i = 1;i <= t;i++)
	{
		cout << instruction[i].name;//对齐指令 
		for(int j = 1;j <= 14 - instruction[i].name.length();j++) printf(" ");
		printf("|");
		//输出流出时间 
		if (instruction[i].issue != 0) printf("%4d|",instruction[i].issue);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		
//		 
//		if (instruction[i].read != 0) printf("%12d|",instruction[i].read);
//		else 
//		{
//			for(int j = 1;j <= 12;j++) printf(" ");
//			printf("|");
//		}
		// 输出执行周期 
		if (instruction[i].exeution != 0) printf("%4d|",instruction[i].exeution);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		//输出写结果周期 
		if (instruction[i].write != 0) printf("%6d|",instruction[i].write);
		else 
		{
			for(int j = 1;j <= 6;j++) printf(" ");
			printf("|");
		}
		
		printf("\n");
		// 输出确认周期 
		if (instruction[i].commit != 0) printf("%4d|",instruction[i].commit);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		
		
	}
	printf("\n");
	//输出功能部件状态表 
	printf("保留站:\n");
	printf("名称|Busy |Op   |Vj |Vk |Qj |Qk |Dest|A |\n");
	for(int i = 1;i <= 6;i++)
	{
		cout << mapp[i];
		for(int j = 1;j <= 10 - mapp[i].length();j++) printf(" ");
		printf("|");
		if (device[i] != 0) printf("true |");// 占用一个保留站 ，device不为0，可以被占用 ，device 是一个数组，用于记录每个功能部件（保留站）的状态，即是否被占用以及被占用的指令编号。 
		else 
		{
			printf("false|     |   |   |   |   |    |  |\n");//没有空闲的保留站，未被占用的保留站 
			continue;
		}
		cout << instruction[device[i]].op;//操作码 
		for(int j = 1;j <= 5 - instruction[device[i]].op.length();j++) printf(" ");
		printf("|");
// 
		if (instruction[device[i]].op[0] == 'L') //load指令 
		{
			if (instruction[device[i]].read != 0 && instruction[device[i]].read != x) printf("F%-2d|R%-2d|   |     |     |no |   |",instruction[device[i]].fi,instruction[device[i]].fj,instruction[device[i]].fk);
			else printf("F%-2d|R%-2d|   |     |     |yes|   |",instruction[device[i]].fi,instruction[device[i]].fj,instruction[device[i]].fk);
		}
		else 
		{
			printf("F%-2d|F%-2d|F%-2d|",instruction[device[i]].fi,instruction[device[i]].fj,instruction[device[i]].fk);
			if (instruction[device[i]].rj == 0 && (instruction[device[i]].read == 0 || instruction[device[i]].read == x))
			{
				std::cout << mapp[instruction[reg[instruction[device[i]].fj]].device];
				for(int j = 1;j <= 5 - mapp[instruction[reg[instruction[device[i]].fj]].device].length();j++) printf(" ");
				printf("|");
			}
			else printf("     |");
			if (instruction[device[i]].rk == 0 && (instruction[device[i]].read == 0 || instruction[device[i]].read == x))
			{
				std::cout << mapp[instruction[reg[instruction[device[i]].fk]].device];
				for(int j = 1;j <= 5 - mapp[instruction[reg[instruction[device[i]].fk]].device].length();j++) printf(" ");
				printf("|");
			}
			else printf("     |");
			if (instruction[device[i]].rj == 0) printf("no |");
			else printf("yes|");
			if (instruction[device[i]].rk == 0) printf("no |");
			else printf("yes|");
		}
		printf("\n");
	}
	printf("\n");
	//输出结果寄存器表 
	printf("结果寄存器:\n");
	printf("F0|F1|F2|F3|F4|F5|F6|F7|F8|F9|F10|F11|F12|F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|F25|F26|F27|F28|F29|F30|\n");
	for(int j = 1;j <= 3;j++)
	{
		for(int i = 0;i <= 30;i++)
		{
			if (instruction[reg[i]].device != 0)
				std::cout << m[j][instruction[reg[i]].device];
			else printf("  ");
			if (i >= 10) printf(" ");
			printf("|");
		}
		printf("\n");
	}
}

