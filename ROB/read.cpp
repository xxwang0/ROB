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
    string Op;   // �������ͣ��� "LOAD", "ADD", "MULT"
    int Vj, Vk;  // ������ֵ
    int Qj, Qk;  // ������״̬
    int Dest;    // ����Ĵ���
    int A;       // ��������ƫ����
};

struct ROB_Entry {
    bool Busy;       // �Ƿ�ռ��
    string Op;       // ��������
    int Dest;        // Ŀ��Ĵ���
    int Value;       // ���ֵ
    bool Ready;      // �Ƿ����
    int Address;     // �ڴ��ַ��������
};

struct instr  //���ָ��ĸ���ϸ�� 
{
	std::string op;
	int fi,fj,fk,time,A;
	int issue,exeution,write,commit;
	std::string name;
} instruction[101];


// vector<instruction> instructions;
vector<ReservationStation> LoadRS(2);   // Load����վ��������2��
vector<ReservationStation> AddRS(2);    // �ӷ�����վ��������2��
vector<ReservationStation> MultRS(2);   // �˷�����վ��������2��
vector<ROB_Entry> ROB(6);               // ����ROB��6����Ŀ
vector<int> RegisterStatus(32, -1);     // �Ĵ���״̬����ʼ��Ϊ-1��ʾδ��ռ��

queue<int> InstructionQueue;  // ָ�����

int cycle = 1;  // ��ǰ������
int current_instruction = 0;  // ��ǰ�����ָ������

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
            for (int j = 2; j < s.length(); j++) {
                if (s[j] == ',') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].A = s[cnt + 1] - '0'; // Ŀ�ļĴ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].A *= 10; // ���Ŀ��Ĵ����������λ��������б���
                instruction[i].A += s[cnt + 2] - '0'; // ���ϸ�λ��
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
    for (int i = 1; i <= t; ++i) {
        cout << instruction[i].op << " " <<instruction[i].fi << " " <<instruction[i].fj << " " <<instruction[i].fk << " " <<instruction[i].A << " " << instruction[i].name << endl;
    }
return 0;
}


