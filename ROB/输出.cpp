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
    std::string Op;  // 操作类型，如 "LOAD", "ADD", "MULT"
    int Vj, Vk;  // 操作数值
    int Qj, Qk;  // 操作数状态
    int Dest;    // 结果寄存器
    int A;       // 立即数或偏移量
};
ReservationStation LoadRS[2];   // Load保留站，假设有2个
ReservationStation AddRS[2];    // 加法保留站，假设有2个
ReservationStation MultRS[2];   // 乘法保留站，假设有2个
struct ROB
{
	int busy;
    std::string op; // 指令名称
    int Dest;
    int value;
    int ready; 
    int Address;
}rob[101];
struct instr  //存放指令的各个细节 
{
	std::string op;
	int fi,fj,fk,time,device,Qj,Qk,Dest,A;
	int issue,exeution,write,commit;
	std::string name;
} instruction[101];
 //定义寄存器状态结构体
struct Register {
    bool busy; // 表示寄存器是否被占用
    int rob; // 表示该寄存器正在等待哪个 ROB 项完成
    int value; // 寄存器的当前值
}f[101];
std::queue<int> q;//指令队列（发射队列）
std::map<std::string,int> map;//用来做映射，将数字与具体指代指令联系 
std::map<int,std::string> mapp;
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





printf("                                                周期1:\n"); 
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
		
		
		// 输出确认周期 
		if (instruction[i].commit != 0) printf("%4d|",instruction[i].commit);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		printf("\n");		
	}
  printf("\n");
   //输出保留站状态表 
   printf("保留站:\n");
   printf(" 名称|Busy |Op   |Vj |Vk |Qj |Qk |Dest|A |\n");
  
   	printf("load1|false| Load|   |   |   |   |    |  |\n");
   	printf("load2|false| Load|   |   |   |   |    |  |\n");
   	printf(" add1|false|  ADD|   |   |   |   |    |  |\n");
   	printf(" add2|false|  ADD|   |   |   |   |    |  |\n");
   	printf("mult1|false| mult|   |   |   |   |    |  |\n");
   	printf("mult2|false| mult|   |   |   |   |    |  |\n");
   	
   	
   	//输出ROB状态表 
   printf("ROB状态表:\n");
   printf("序号|Busy |Op   |Dest | 值 |Ready|\n");
   	printf("   1|false|     |     |    |     |\n");
   	printf("   2|false|     |     |    |     |\n");
   	printf("   3|false|     |     |    |     |\n");
   	printf("   4|false|     |     |    |     |\n");
   	printf("   5|false|     |     |    |     |\n");
   	printf("   6|false|     |     |    |     |\n");


   printf("\n");
   //输出结果寄存器表 
   printf("结果寄存器:\n");
   printf(" 字段|F0|F1|F2|F3|F4|F5|F6|F7|F8|F9|F10|F11|F12|F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|F25|F26|F27|F28|F29|F30|\n");
   printf(" busy|\n");
   printf("  rob|\n");
   printf("value|\n");
}
