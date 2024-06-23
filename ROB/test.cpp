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
    string op;   // �������ͣ��� "LOAD", "ADD", "MULT"
    int Vj, Vk;  // ������ֵ
    int Qj, Qk;  // ������״̬
    int Dest;    // ����Ĵ���
    int A;       // ��������ƫ����
};

struct ROB_Entry {
    bool Busy;       // �Ƿ�ռ��
    string op;       // ��������
    int Dest;        // Ŀ��Ĵ���
    int Value;       // ���ֵ
    bool Ready;      // �Ƿ����
    int Address;     // �ڴ��ַ��������
};

struct instr  //���ָ��ĸ���ϸ�� 
{
	std::string op;
	int fi,fj,fk,time,A;
	int issue,execute,write,commit;
	std::string name;
} instruction[101];


// vector<instruction> instructions;
vector<ReservationStation> LoadRS(2);   // Load����վ��������2��
vector<ReservationStation> AddRS(2);    // �ӷ�����վ��������2��
vector<ReservationStation> MultRS(2);   // �˷�����վ��������2��
vector<ROB_Entry> ROB(6);               // ����ROB��6����Ŀ
vector<int> RegisterStatus(32, -1);     // �Ĵ���״̬����ʼ��Ϊ-1��ʾδ��ռ��

queue<int> q;  // ָ�����

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
    
for (int i = 1; i <= t; i++) q.push(i);
    //cycle++;
    while (!q.empty()) {
        int x = q.front();
        if (instruction[x].op == "LD") {  // ������������ǡ�LD�������أ�
            for (int i = 0; i < LoadRS.size(); ++i) {  // �������ر���վ
                if (!LoadRS[i].Busy) {  // ����ñ���վδ��ռ��
                    LoadRS[i].Busy = true;  // ��Ǳ���վΪռ��״̬
                    LoadRS[i].op = instruction[x].op;  // ���ò�������
                    LoadRS[i].Dest = instruction[x].fi;  // ����Ŀ��Ĵ���
                    LoadRS[i].A = instruction[x].A;  // ������������ƫ����

                    // ���Դ������״̬Ϊ���ã����Ĵ���״̬���ж�Ӧ�Ĵ���״̬Ϊ-1����ֱ�Ӷ�ȡ������ֵ
                    if (RegisterStatus[instruction[x].fj] == -1) {
                        LoadRS[i].Vj = instruction[x].fj;
                        LoadRS[i].Qj = -1;
                    } else {  // �������ñ���վ�в�����״̬Ϊ�Ĵ���״̬���е�ֵ
                        LoadRS[i].Qj = RegisterStatus[instruction[x].fj];
                    }

                    // ���Ŀ��Ĵ���Ϊռ��״̬
                    RegisterStatus[instruction[x].fi] = i;

                    // ����ָ�������
                    instruction[x].issue = cycle;
                    cycle++;

                    // ��ָ��Ӷ����е���
                    q.pop();

                    // ����ѭ��������������һ��ָ��
                    break;
                }
            }
        } else {  // ����������Ͳ��ǡ�LD������Ϊ��������ָ�
            if (instruction[x].op == "ADDD" || instruction[x].op == "SUBD") {  // ����Ǽӷ������ָ��
                for (int i = 0; i < AddRS.size(); ++i) {  // �����ӷ�����վ
                    if (!AddRS[i].Busy) {  // ����ñ���վδ��ռ��
                        AddRS[i].Busy = true;  // ��Ǳ���վΪռ��״̬
                        AddRS[i].op = instruction[x].op;  // ���ò�������
                        AddRS[i].Dest = instruction[x].fi;  // ����Ŀ��Ĵ���

                        // ���Դ������״̬Ϊ���ã�ֱ�Ӷ�ȡ������ֵ
                        if (RegisterStatus[instruction[x].fj] == -1) {
                            AddRS[i].Vj = instruction[x].fj;
                            AddRS[i].Qj = -1;
                        } else {
                            AddRS[i].Qj = RegisterStatus[instruction[x].fj];
                        }

                        // ���Դ������״̬Ϊ���ã�ֱ�Ӷ�ȡ������ֵ
                        if (RegisterStatus[instruction[x].fk] == -1) {
                            AddRS[i].Vk = instruction[x].fk;
                            AddRS[i].Qk = -1;
                        } else {
                            AddRS[i].Qk = RegisterStatus[instruction[x].fk];
                        }

                        // ���Ŀ��Ĵ���Ϊռ��״̬
                        RegisterStatus[instruction[x].fi] = i;

                        // ����ָ�������
                        instruction[x].issue = cycle;
cycle++;

                        // ��ָ��Ӷ����е���
                        q.pop();

                        // ����ѭ��������������һ��ָ��
                        break;
                    }
                }
            } else {  // ����ǳ˷������ָ��
                for (int i = 0; i < MultRS.size(); ++i) {  // �����˷�����վ
                    if (!MultRS[i].Busy) {  // ����ñ���վδ��ռ��
                        MultRS[i].Busy = true;  // ��Ǳ���վΪռ��״̬
                        MultRS[i].op = instruction[x].op;  // ���ò�������
                        MultRS[i].Dest = instruction[x].fi;  // ����Ŀ��Ĵ���

                        // ���Դ������״̬Ϊ���ã�ֱ�Ӷ�ȡ������ֵ
                        if (RegisterStatus[instruction[x].fj] == -1) {
                            MultRS[i].Vj = instruction[x].fj;
                            MultRS[i].Qj = -1;
                        } else {
                            MultRS[i].Qj = RegisterStatus[instruction[x].fj];
                        }

                        // ���Դ������״̬Ϊ���ã�ֱ�Ӷ�ȡ������ֵ
                        if (RegisterStatus[instruction[x].fk] == -1) {
                            MultRS[i].Vk = instruction[x].fk;
                            MultRS[i].Qk = -1;
                        } else {
                            MultRS[i].Qk = RegisterStatus[instruction[x].fk];
                        }

                        // ���Ŀ��Ĵ���Ϊռ��״̬
                        RegisterStatus[instruction[x].fi] = i;

                        // ����ָ�������
                        instruction[x].issue = cycle;
cycle++;
                        // ��ָ��Ӷ����е���
                        q.pop();

                        // ����ѭ��������������һ��ָ��
                        break;
                    }
                }
            }
        }
    }

    for (int i = 1; i <= t; ++i) {
       cout <<instruction[i].issue << endl;
    }
    return 0;
}
