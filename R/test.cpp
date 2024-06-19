#include <cstdio>       // C��׼���������
#include <algorithm>    // ��׼�㷨��
#include <string>       // �ַ�����
#include <iostream>     // ��׼�����������
#include <queue>        // ����������
#include <set>          // ����������
#include <map>          // ӳ��������
#include <windows.h>    // Windowsƽ̨�ض���
#include <cstdlib>      // C��׼��

using namespace std;    // ʹ�ñ�׼��������ռ�

int t;
 
queue<int> q;//ָ����У�������У� 
struct st
{
	int n,step;
};

struct Regist
{
	int busy,value;
} regist[101];

struct instr  //���ָ��ĸ���ϸ�� 
{
	string op;
	int Vj,Vk,time,device,Qj,Qk,Dest,A;
	int issue,exeution,write,commit;
	string name;
} instruction[101];
struct ROB
{
	int busy;
    string op; // ָ������
    int Dest;
    int value;
    int ready; 
    int Address;
}rob[101];

queue<int> reg_q[32];//�����Ҫ�üĴ��������ָ�� 

int reg[32];//���д�üĴ�����ָ�� 
int device[8];//���ռ�øù��ܲ�����ָ�� 
set<int> step[6];//��Ÿ����׶ε�ָ����� 
set<int>::iterator it,ite;//������������ö�� 
map<string,int> map;//������ӳ�䣬�����������ָ��ָ����ϵ 
map<int,string> mapp;//������ӳ�䣬�����������ָ��ָ����ϵ
string m[4][32];//������ӳ�䣬�����������ָ��ָ����ϵ

//��ʼ��

inline void init()
{
	map["LD"] = 1;map["SUBD"] = 2;map["ADDD"] = 2;map["DIVD"] = 3;map["MULTD"] = 4;//������ӳ�䣬�����������ָ��ָ����ϵ ,������ 
	mapp[1] = "Load1";mapp[2] = "Load2";mapp[3] = "Add1";mapp[4] = "Add2";mapp[5] = "Mult1"; mapp[6] = "Mult2";//����վ 
//	m[1][1] = "��";m[2][1] = "��";m[3][1] = "  ";
//	m[1][2] = "��";m[2][2] = "��";m[3][2] = "  "; 
//	m[1][3] = "��";m[2][3] = "��";m[3][3] = "  "; 
//	m[1][4] = "��";m[2][4] = "��";m[3][4] = "1 "; 
//	m[1][5] = "��";m[2][5] = "��";m[3][5] = "2 ";
}

// ��ȡָ�����
inline void read() {
    // ����Ҫִ�е�ָ����
    scanf("%d", &t);

    // ȥ�����໻�з�����ֹ�������
    char ch;
    while (1) {
        ch = getchar();
        if (ch == '\n') break;
    }

    // ��������ָ��
    for (int i = 1; i <= t; i++) {
        string s;
        getline(cin, s); // ��ȡһ��ָ���ַ���

        instruction[i].name = s; // ��ԭʼָ���ַ����洢�� instruction �ṹ���е� name �ֶ�

        // ����ָ�������ĸ�ж�ָ�����Ͳ�����
        if (s[0] == 'L') {
            // ���� LD ָ��
            instruction[i].op = "LD"; // ����ָ���������Ϊ LD
            instruction[i].time = 2; // LD ָ��ִ������Ϊ 2 ������

            // ���� fi��Ŀ��Ĵ������
            int cnt = 0;
            for (int j = 2; j < s.length(); j++) {
                if (s[j] != ' ') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fi = s[cnt + 1] - '0'; // Ŀ�ļĴ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fi *= 10; // ���Ŀ��Ĵ����������λ��������б���
                instruction[i].fi += s[cnt + 2] - '0'; // ���ϸ�λ��
            }

            // ���� fj��Դ�������Ĵ������
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
                m*= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].Vj = R[m];
            }
        } 
		else {
            // �����������͵�ָ�MULTD, SUBD, ADDD, DIVD��
            int cnt = 0;
            if (s[0] == 'M') {
                instruction[i].op = "MULTD"; // ����ָ���������Ϊ MULTD
                instruction[i].time = 10; // MULTD ָ��ִ��ʱ����Ϊ 10 ������
            } else if (s[0] == 'S') {
                instruction[i].op = "SUBD"; // ����ָ���������Ϊ SUBD
                instruction[i].time = 2; // SUBD ָ��ִ��ʱ����Ϊ 2 ������
            } else if (s[0] == 'A') {
                instruction[i].op = "ADDD"; // ����ָ���������Ϊ ADDD
                instruction[i].time = 2; // ADDD ָ��ִ��ʱ����Ϊ 2 ������
            } else if (s[0] == 'D') {
                instruction[i].op = "DIVD"; // ����ָ���������Ϊ DIVD
                instruction[i].time = 20; // DIVD ָ��ִ��ʱ����Ϊ 40 ������
            }

            // ���� fi��Ŀ��Ĵ������
            for (int j = 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fi = s[cnt + 1] - '0'; // ��ȡĿ��Ĵ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fi *= 10; // ���Ŀ��Ĵ����������λ��������б���
                instruction[i].fi += s[cnt + 2] - '0'; // ���ϸ�λ��
            }

            // ���� fj��Դ�������Ĵ������
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            m= s[cnt + 1] - '0';
            instruction[i].Vj = R[m] // ��ȡԴ�������Ĵ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].Vj = R[m]
            }

            // ���� Vk���ڶ���Դ�������Ĵ������
           for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            m= s[cnt + 1] - '0';
            instruction[i].Vk = R[m] // ��ȡԴ�������Ĵ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].Vk = R[m]
            }
        }
    }
}

//������ 
inline void print(int x)
{
	printf("                                                ����%d:\n",x); 
	//���ָ��״̬�� 
	printf("ָ��״̬��:\n");
	printf("ָ����        |����|ִ��|д���|ȷ��|\n");
	for(int i = 1;i <= t;i++)
	{
		cout << instruction[i].name;//����ָ�� 
		for(int j = 1;j <= 14 - instruction[i].name.length();j++) printf(" ");
		printf("|");
		//�������ʱ�� 
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
		// ���ִ������ 
		if (instruction[i].exeution != 0) printf("%4d|",instruction[i].exeution);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		//���д������� 
		if (instruction[i].write != 0) printf("%6d|",instruction[i].write);
		else 
		{
			for(int j = 1;j <= 6;j++) printf(" ");
			printf("|");
		}
		
		printf("\n");
		// ���ȷ������ 
		if (instruction[i].commit != 0) printf("%4d|",instruction[i].commit);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		
		
	}
	printf("\n");
	//������ܲ���״̬�� 
	printf("����վ:\n");
	printf("����|Busy |Op   |Vj |Vk |Qj |Qk |Dest|A |\n");
	for(int i = 1;i <= 6;i++)
	{
		cout << mapp[i];
		for(int j = 1;j <= 10 - mapp[i].length();j++) printf(" ");
		printf("|");
		if (device[i] != 0) printf("true |");// ռ��һ������վ ��device��Ϊ0�����Ա�ռ�� ��device ��һ�����飬���ڼ�¼ÿ�����ܲ���������վ����״̬�����Ƿ�ռ���Լ���ռ�õ�ָ���š� 
		else 
		{
			printf("false|     |   |   |   |   |    |  |\n");//û�п��еı���վ��δ��ռ�õı���վ 
			continue;
		}
		cout << instruction[device[i]].op;//������ 
		for(int j = 1;j <= 5 - instruction[device[i]].op.length();j++) printf(" ");
		printf("|");
// 
		if (instruction[device[i]].op[0] == 'L') //loadָ�� 
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
	//�������Ĵ����� 
	printf("����Ĵ���:\n");
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

