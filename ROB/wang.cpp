#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

struct ReservationStation {
    bool Busy;   // �Ƿ�ռ��
    string op;   // ��������
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

struct instr {  // ���ָ��ĸ���ϸ��
    string op;
    int fi, fj, fk, time, A;
    int issue, execute, write, commit;
    string name;
} instruction[101];

// vector<instruction> instructions;
vector<ReservationStation> LoadRS(2);   // Load����վ��������2��
vector<ReservationStation> AddRS(2);    // �ӷ�����վ��������2��
vector<ReservationStation> MultRS(2);   // �˷�����վ��������2��
vector<ROB_Entry> ROB(6);               // ����ROB��6����Ŀ
vector<int> RegisterStatus(32, -1);     // �Ĵ���״̬����ʼ��Ϊ-1��ʾδ��ռ��

queue<int> q;  // ָ�����

int cycle = 0;  // ��ǰ������
int time = 1;
int t; // ָ������

void read() {
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
        if (s[0] == 'L' || s[0] == 'S') {
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
                    cnt = j;
                    m = s[cnt + 1] - '0';
                    instruction[i].fj = m;
                    break;
                }
            }
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
                instruction[i].fj = m;
            }

        } else {
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
                instruction[i].time = 20; // DIVD ָ��ִ��ʱ����Ϊ 20 ������
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
            int m = s[cnt + 1] - '0';
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
            m = s[cnt + 1] - '0';
            instruction[i].fk = m; // ��ȡԴ�������Ĵ�����ŵ�ʮλ��
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                m *= 10; // ���Դ�������Ĵ����������λ��������б���
                m += s[cnt + 2] - '0'; // ���ϸ�λ��
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

                        // ��ָ��Ӷ����е���
                        q.pop();

                        // ����ѭ��������������һ��ָ��
                        break;
                    }
                }
            }
        }
    }
}

void Execute() {
    // ִ�н׶Σ�����ÿ������Ϊһ���������

    // �������ر���վ
    for (int i = 0; i < LoadRS.size(); ++i) {
        if (LoadRS[i].Busy) {  // ����ü��ر���վ��ռ��
            // ģ��LD����������ִ�����
            ROB[LoadRS[i].Dest].Value = LoadRS[i].Vj + LoadRS[i].A;  // ������ֵ��д��ROB
            ROB[LoadRS[i].Dest].Ready = true;  // ���ROB��ĿΪ����״̬
            instruction[LoadRS[i].Dest].execute = cycle;  // ��¼ָ��ִ�е�������

            LoadRS[i].Busy = false;  // ������ر���վ״̬
            
        }
    }

    // �����ӷ�����վ
    for (int i = 0; i < AddRS.size(); ++i) {
        if (AddRS[i].Busy) {  // ����üӷ�����վ��ռ��
            // ģ��ADD����������ִ�����
            ROB[AddRS[i].Dest].Value = AddRS[i].Vj + AddRS[i].Vk;  // ������ֵ��д��ROB
            ROB[AddRS[i].Dest].Ready = true;  // ���ROB��ĿΪ����״̬
            instruction[AddRS[i].Dest].execute = cycle;  // ��¼ָ��ִ�е�������

            AddRS[i].Busy = false;  // ����ӷ�����վ״̬
        }
       
    }

    // �����˷�����վ
    for (int i = 0; i < MultRS.size(); ++i) {
        if (MultRS[i].Busy) {  // ����ó˷�����վ��ռ��
            // ģ��MULT����������ִ�����
            ROB[MultRS[i].Dest].Value = MultRS[i].Vj * MultRS[i].Vk;  // ������ֵ��д��ROB
            ROB[MultRS[i].Dest].Ready = true;  // ���ROB��ĿΪ����״̬
            instruction[MultRS[i].Dest].execute = cycle;  // ��¼ָ��ִ�е�������

            MultRS[i].Busy = false;  // ����˷�����վ״̬
        }
        
    }

    cycle+=2;  // ��������1��������һ������
}


void Write() {
    // д����׶Σ�����д��Ϊһ���������
    for (int i = 0; i < ROB.size(); ++i) {
        if (ROB[i].Busy && ROB[i].Ready) {  // ���ROB��Ŀ��ռ���Ҿ���
            int dest_reg = ROB[i].Dest;
            RegisterStatus[dest_reg] = ROB[i].Value;  // ��ROB�еĽ��ֵд��Ŀ��Ĵ���
            ROB[i].Busy = false;  // ���ROB��ĿΪδռ��
            instruction[i].write = cycle;  // ��¼ָ��д�ص�������
            
        }
    }
    cycle++;
}


void Commit() {
    int i = 0;
    bool committed = false; // ����Ƿ��Ѿ�ȷ����һ��ָ��

    while (i < ROB.size() && !committed) {
        if (ROB[i].Ready && ROB[i].Busy) { // ���ROB��Ŀ�Ƿ�����ұ�ռ��
            int d = ROB[i].Dest;
            RegisterStatus[d] = ROB[i].Value; // ��ROB�еĽ��д��Ŀ��Ĵ���
            ROB[i].Busy = false; // ���ROB��ĿΪδռ��
            ROB[i].Ready = false; // ���ROB��ĿΪδ����

            // ��ROB��ɾ����ָ��
            ROB.erase(ROB.begin() + i);

            instruction[i].commit = cycle; // ��¼ָ���ύ��������
            committed = true; // �����ȷ����һ��ָ��
        } else {
            i++; // �����ǰROB��Ŀ�����������������һ����Ŀ
        }
    }
    cycle++;
}


void Print() {
    // ���ָ��״̬��
    	printf("                                                ����%d:\n",&cycle); 
    printf("ָ����        |����|ִ��|д���|ȷ��|\n");
    for (int i = 1; i <= t; i++) {
        cout << instruction[i].name; // ����ָ����
        for (int j = 1; j <= 14 - instruction[i].name.length(); j++) printf(" ");
        printf("|");
        // �������ʱ��
        if (instruction[i].issue != 0) printf("%4d|", instruction[i].issue);
        else printf("    |");
        // ���ִ������
        if (instruction[i].execute != 0) printf("%4d|", instruction[i].execute);
        else printf("    |");
        // ���д�������
        if (instruction[i].write != 0) printf("%6d|", instruction[i].write);
        else printf("      |");
        // ���ȷ������
        if (instruction[i].commit != 0) printf("%4d|", instruction[i].commit);
        else printf("    |");
        printf("\n");
    }
    printf("\n");

    // �������վ״̬��
    printf("����վ:\n");
    printf(" ����|Busy |   Op|Vj |Vk |Qj |Qk |Dest|  A|\n");
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

    // ���ROB״̬��
    printf("ROB״̬��:\n");
    printf("���|Busy |Op   |Dest | ֵ |Ready |\n");
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

    // �������Ĵ�����
    printf("����Ĵ���:\n");
    printf(" �ֶ� |F0|F1|F2|F3|F4|F5|F6|F7|F8|F9\n");
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




