#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <cstdlib>

int t;

std::queue<int> q; // ָ����У�������У�
struct st {
    int n, step;
};

struct instr { // ���ָ��ĸ���ϸ��
    std::string op;
    int fi, fj, fk, time, device, rj, rk, qj, qk;
    int issue, read, exeution, write;
    std::string name;
} instruction[100 + 1];

std::queue<int> reg_q[30 + 1]; // �����Ҫ�üĴ��������ָ��
int reg[31 + 1]; // ���д�üĴ�����ָ��
int device[5 + 1]; // ���ռ�øù��ܲ�����ָ��
std::set<int> step[6]; // ��Ÿ����׶ε�ָ�����
std::set<int>::iterator it, ite; // ������������ö��
std::map<std::string, int> mapVar; // ������ӳ�䣬�����������ָ��ָ����ϵ
std::map<int, std::string> mapp; // ������ӳ�䣬�����������ָ��ָ����ϵ
std::string m[3 + 1][30 + 1]; // ������ӳ�䣬�����������ָ��ָ����ϵ

// ��ʼ��
inline void init() {
    mapVar["LD"] = 1; mapVar["SUBD"] = 2; mapVar["ADDD"] = 2; mapVar["DIVD"] = 3; mapVar["MULTD"] = 4;
    mapp[1] = "����"; mapp[2] = "�ӷ�"; mapp[3] = "����"; mapp[4] = "�˷�1"; mapp[5] = "�˷�2";
    m[1][1] = "��"; m[2][1] = "��"; m[3][1] = "  ";
    m[1][2] = "��"; m[2][2] = "��"; m[3][2] = "  ";
    m[1][3] = "��"; m[2][3] = "��"; m[3][3] = "  ";
    m[1][4] = "��"; m[2][4] = "��"; m[3][4] = "1 ";
    m[1][5] = "��"; m[2][5] = "��"; m[3][5] = "2 ";
}

// ��ָ��
inline void read() {
    // ����Ҫִ�е�ָ����
    scanf("%d", &t);
    // ȥ�����໻�з�����ֹ�������
    char ch;
    while (1) {
        ch = getchar();
        if (ch == '\n') break;
    }

    for (int i = 1; i <= t; i++) {
        std::string s;
        getline(std::cin, s);
        instruction[i].name = s;
        // ����LDָ��
        if (s[0] == 'L') {
            instruction[i].op = "LD";
            instruction[i].time = 1;
            int cnt = 0;
            for (int j = 2; j < s.length(); j++) {
                if (s[j] != ' ') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fi = s[cnt + 1] - '0';
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fi *= 10;
                instruction[i].fi += s[cnt + 2] - '0';
            }
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'R') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fj = s[cnt + 1] - '0';
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fj *= 10;
                instruction[i].fj += s[cnt + 2] - '0';
            }
        }
        // ��������ָ��
        else {
            int cnt = 0;
            if (s[0] == 'M') {
                instruction[i].op = "MULTD";
                instruction[i].time = 10;
            } else if (s[0] == 'S') {
                instruction[i].op = "SUBD";
                instruction[i].time = 2;
            } else if (s[0] == 'A') {
                instruction[i].op = "ADDD";
                instruction[i].time = 2;
            } else if (s[0] == 'D') {
                instruction[i].op = "DIVD";
                instruction[i].time = 40;
            }
            for (int j = 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fi = s[cnt + 1] - '0';
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fi *= 10;
                instruction[i].fi += s[cnt + 2] - '0';
            }
            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fj = s[cnt + 1] - '0';
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fj *= 10;
                instruction[i].fj += s[cnt + 2] - '0';
            }

            for (int j = cnt + 1; j < s.length(); j++) {
                if (s[j] == 'F') {
                    cnt = j;
                    break;
                }
            }
            instruction[i].fk = s[cnt + 1] - '0';
            if (s[cnt + 2] >= '0' && s[cnt + 2] <= '9') {
                instruction[i].fk *= 10;
                instruction[i].fk += s[cnt + 2] - '0';
            }
        }
    }
}

// ������
inline void print(int x) {
    printf("                                                ����%d:\n", x);
    // ���ָ��״̬��
    printf("ָ��״̬:\n");
    printf("ָ����        |Issue|Read operand|Execution completed|Write Result|\n");
    for (int i = 1; i <= t; i++) {
        std::cout << instruction[i].name;
        for (int j = 1; j <= 14 - instruction[i].name.length(); j++) printf(" ");
        printf("|");
        if (instruction[i].issue != 0) printf("%5d|", instruction[i].issue);
        else {
            for (int j = 1; j <= 5; j++) printf(" ");
            printf("|");
        }
        if (instruction[i].read != 0) printf("%12d|", instruction[i].read);
        else {
            for (int j = 1; j <= 12; j++) printf(" ");
            printf("|");
        }

        if (instruction[i].exeution != 0) printf("%19d|", instruction[i].exeution);
        else {
            for (int j = 1; j <= 19; j++) printf(" ");
            printf("|");
        }

        if (instruction[i].write != 0) printf("%12d|", instruction[i].write);
        else {
            for (int j = 1; j <= 12; j++) printf(" ");
            printf("|");
        }

        printf("\n");

    }
    printf("\n");
    // ������ܲ���״̬��
    printf("���ܲ���״̬:\n");
    printf("���ܲ�����|Busy |Op   |Fi |Fj |Fk |Qj   |Qk   |Rj |Rk |\n");
    for (int i = 1; i <= 5; i++) {
        std::cout << mapp[i];
        for (int j = 1; j <= 9 - mapp[i].length(); j++) printf(" ");
        printf("|");
        if (device[i] == 0) printf("  NO  |");
        else {
            printf("  YES  |");
            std::cout << m[i][device[i]].c_str();
            for (int j = 1; j <= 4 - m[i][device[i]].length(); j++) printf(" ");
            printf("|");
            printf("%3d|", instruction[device[i]].fi);
            printf("%3d|", instruction[device[i]].fj);
            printf("%3d|", instruction[device[i]].fk);
            if (instruction[device[i]].qj != 0) printf("%4d |", instruction[device[i]].qj);
            else {
                printf("  NO |");
                for (int j = 1; j <= 4; j++) printf(" ");
                printf("|");
            }
            if (instruction[device[i]].qk != 0) printf("%4d |", instruction[device[i]].qk);
            else {
                printf("  NO |");
                for (int j = 1; j <= 4; j++) printf(" ");
                printf("|");
            }
            if (instruction[device[i]].rj != 0) printf("%3d|", instruction[device[i]].rj);
            else {
                printf("  NO|");
                for (int j = 1; j <= 3; j++) printf(" ");
                printf("|");
            }
            if (instruction[device[i]].rk != 0) printf("%3d|", instruction[device[i]].rk);
            else {
                printf("  NO|");
                for (int j = 1; j <= 3; j++) printf(" ");
                printf("|");
            }
        }
        printf("\n");
    }
    printf("\n");

    // ����Ĵ���״̬��
    printf("�Ĵ���״̬:\n");
    for (int i = 1; i <= 30; i++) {
        std::cout << "F";
        if (i < 10) std::cout << 0;
        std::cout << i << "  ";
        for (int j = 1; j <= 3; j++) printf("|");
        if (reg_q[i].empty()) printf("  NO  |");
        else {
            printf("  YES  |");
            printf("  %3d|", reg_q[i].front());
        }
        if (reg[i] == 0) printf("  NO  |\n");
        else {
            printf("  YES  |");
            printf("  %3d|\n", reg[i]);
        }
    }
    printf("\n");
}

inline void quit() {
    for (int i = 1; i <= t; i++) {
        instruction[i].issue = 0;
        instruction[i].read = 0;
        instruction[i].exeution = 0;
        instruction[i].write = 0;
    }
    for (int i = 1; i <= 5; i++) device[i] = 0;
    for (int i = 1; i <= 30; i++) {
        while (reg_q[i].size()) reg_q[i].pop();
        reg[i] = 0;
    }
    for (int i = 1; i <= 5; i++) step[i].clear();
    for (int i = 1; i <= 100; i++) instruction[i].time = 0;
}

inline void func(int x) {
    int u = 0, v = 0;
    while (x == 1) {
        while (q.size()) q.pop();
        for (int i = 1; i <= t; i++) {
            if (instruction[i].issue == 0) {
                q.push(i);
            }
        }
        for (int i = 1; i <= 5; i++) {
            if (device[i] == 0) {
                if (q.size()) {
                    v = q.front();
                    q.pop();
                    if (step[i].count(v) == 0) {
                        if (instruction[v].op == "LD") {
                            instruction[v].rj = instruction[v].fj;
                            instruction[v].qj = instruction[v].fi;
                            if (instruction[instruction[v].qj].write != 0) instruction[v].rj = instruction[instruction[v].qj].write;
                            else {
                                instruction[v].qj = 0;
                                instruction[v].rj = 0;
                            }
                        } else {
                            instruction[v].rj = instruction[v].fj;
                            instruction[v].qj = instruction[v].fi;
                            if (instruction[instruction[v].qj].write != 0) instruction[v].rj = instruction[instruction[v].qj].write;
                            else {
                                instruction[v].qj = 0;
                                instruction[v].rj = 0;
                            }

                            instruction[v].rk = instruction[v].fk;
                            instruction[v].qk = instruction[v].fi;
                            if (instruction[instruction[v].qk].write != 0) instruction[v].rk = instruction[instruction[v].qk].write;
                            else {
                                instruction[v].qk = 0;
                                instruction[v].rk = 0;
                            }
                        }
                        device[i] = v;
                        instruction[v].issue = u + 1;
                        step[i].insert(v);
                        instruction[v].read = u + 1;
                        u = u + 1 + 1;
                    }
                }
            } else {
                if (instruction[device[i]].exeution + 1 == instruction[device[i]].time) {
                    instruction[device[i]].write = u + 1;
                    reg[instruction[device[i]].fi] = instruction[device[i]].time;
                    step[i].erase(device[i]);
                    device[i] = 0;
                    u = u + 1 + 1;
                }
            }
        }
        for (int i = 1; i <= 5; i++) {
            if (device[i] != 0) instruction[device[i]].exeution = instruction[device[i]].exeution + 1;
        }
    }
}

// ������
int main() {
    // ��ʼ��
    init();
    // ����
    read();
    // ����
    func(1);
    // ���
    print(1);
    quit();
    return 0;
}
2
ADDD F2,F6,F4
LD F2,45(R3)
