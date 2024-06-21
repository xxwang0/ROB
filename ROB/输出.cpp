#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>
using namespace std;
struct ReservationStation {
    bool Busy;   // �Ƿ�ռ��
    std::string Op;  // �������ͣ��� "LOAD", "ADD", "MULT"
    int Vj, Vk;  // ������ֵ
    int Qj, Qk;  // ������״̬
    int Dest;    // ����Ĵ���
    int A;       // ��������ƫ����
};
ReservationStation LoadRS[2];   // Load����վ��������2��
ReservationStation AddRS[2];    // �ӷ�����վ��������2��
ReservationStation MultRS[2];   // �˷�����վ��������2��
struct ROB
{
	int busy;
    std::string op; // ָ������
    int Dest;
    int value;
    int ready; 
    int Address;
}rob[101];
struct instr  //���ָ��ĸ���ϸ�� 
{
	std::string op;
	int fi,fj,fk,time,device,Qj,Qk,Dest,A;
	int issue,exeution,write,commit;
	std::string name;
} instruction[101];
 //����Ĵ���״̬�ṹ��
struct Register {
    bool busy; // ��ʾ�Ĵ����Ƿ�ռ��
    int rob; // ��ʾ�üĴ������ڵȴ��ĸ� ROB �����
    int value; // �Ĵ����ĵ�ǰֵ
}f[101];
std::queue<int> q;//ָ����У�������У�
std::map<std::string,int> map;//������ӳ�䣬�����������ָ��ָ����ϵ 
std::map<int,std::string> mapp;
int main(){


int t; 
scanf("%d", &t);

    // ȥ�����໻�з�����ֹ�������
    char ch;
    while (1) {
        ch = getchar();
        if (ch == '\n') break;
    }

    // ��������ָ��
    for (int i = 1; i <= t; i++) {
        std::string s;
        getline(std::cin, s); // ��ȡһ��ָ���ַ���

        instruction[i].name = s; // ��ԭʼָ���ַ����洢�� instruction �ṹ���е� name �ֶ�

        // ����ָ�������ĸ�ж�ָ�����Ͳ�����
        if (s[0] == 'L'||s[0] == 'S') {
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
                	m=s[cnt + 1] - '0';
                	instruction[i].fj = m;
                    break;
                }
            }
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m*= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].fj = m;
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
            int m= s[cnt + 1] - '0';
            instruction[i].fj = m; // ��ȡԴ�������Ĵ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].fj = m;
            }

            // ���� Vk���ڶ���Դ�������Ĵ������
           for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            m= s[cnt + 1] - '0';
            instruction[i].fk = m; // ��ȡԴ�������Ĵ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].fk = m;
            }
        }
    }





printf("                                                ����1:\n"); 
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
		
		
		// ���ȷ������ 
		if (instruction[i].commit != 0) printf("%4d|",instruction[i].commit);
		else 
		{
			for(int j = 1;j <= 4;j++) printf(" ");
			printf("|");
		}
		printf("\n");		
	}
  printf("\n");
   //�������վ״̬�� 
   printf("����վ:\n");
   printf(" ����|Busy |Op   |Vj |Vk |Qj |Qk |Dest|A |\n");
  
   	printf("load1|false| Load|   |   |   |   |    |  |\n");
   	printf("load2|false| Load|   |   |   |   |    |  |\n");
   	printf(" add1|false|  ADD|   |   |   |   |    |  |\n");
   	printf(" add2|false|  ADD|   |   |   |   |    |  |\n");
   	printf("mult1|false| mult|   |   |   |   |    |  |\n");
   	printf("mult2|false| mult|   |   |   |   |    |  |\n");
   	
   	
   	//���ROB״̬�� 
   printf("ROB״̬��:\n");
   printf("���|Busy |Op   |Dest | ֵ |Ready|\n");
   	printf("   1|false|     |     |    |     |\n");
   	printf("   2|false|     |     |    |     |\n");
   	printf("   3|false|     |     |    |     |\n");
   	printf("   4|false|     |     |    |     |\n");
   	printf("   5|false|     |     |    |     |\n");
   	printf("   6|false|     |     |    |     |\n");


   printf("\n");
   //�������Ĵ����� 
   printf("����Ĵ���:\n");
   printf(" �ֶ�|F0|F1|F2|F3|F4|F5|F6|F7|F8|F9|F10|F11|F12|F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|F25|F26|F27|F28|F29|F30|\n");
   printf(" busy|\n");
   printf("  rob|\n");
   printf("value|\n");
}
