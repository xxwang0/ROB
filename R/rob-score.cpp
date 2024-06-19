#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <vector>
using namespace std;

const int MAX_INSTR = 100;

// ���ܵ�Ԫ����
enum FuncUnitType {
    INT,    // ��������
    ADD,    // �ӷ�
    DIV,    // ����
    MULT1,  // �˷�1
    MULT2   // �˷�2
};

// ָ��ṹ��
struct Instruction {
    string name;    // ָ������
    string op;      // ��������
    int fi, fj, fk; // �������Ĵ������
    int issue, read, execution, write; // ʱ���
    int rob_entry;  // ROB��Ŀ���
    bool rob_done;  // ���ָ���Ƿ����ִ��

    // ���캯����ʼ��
    Instruction() : issue(0), read(0), execution(0), write(0), rob_entry(-1), rob_done(false) {}
};

// ԤԼվ��Reservation Station���ṹ��
struct ReservationStation {
    bool busy;           // �Ƿ�æµ
    FuncUnitType type;   // ���ܵ�Ԫ����
    int op;              // �������ͱ��
    int vj, vk;          // ������ֵ
    int qj, qk;          // ����������״̬��0��ʾ������
    int dest;            // Ŀ�ļĴ���
    int instruction_idx; // ָ����ָ�������е�����

    ReservationStation() : busy(false), type(INT), op(-1), vj(0), vk(0), qj(0), qk(0), dest(0), instruction_idx(-1) {}
};

// ROB��Reorder Buffer���ṹ��
struct ReorderBuffer {
    int head; // ROBͷָ��
    int tail; // ROBβָ��
    vector<int> entries; // ROB��Ŀ

    ReorderBuffer() : head(0), tail(0) {}
};

// ȫ�ֱ���
int t; // ָ������
Instruction instructions[MAX_INSTR + 1]; // ָ������
ReservationStation reservationStations[5]; // ԤԼվ���飨5�����ܵ�Ԫ��
ReorderBuffer rob; // ROB

// ָ���������ӳ��
struct OpTypeMapInitializer {
    map<string, FuncUnitType> operator()() const {
        map<string, FuncUnitType> temp;
        temp["LD"] = INT;
        temp["SUBD"] = ADD;
        temp["ADDD"] = ADD;
        temp["DIVD"] = DIV;
        temp["MULTD"] = MULT1;
        return temp;
    }
};
static OpTypeMapInitializer opTypeMapInitializer;
map<string, FuncUnitType> opTypeMap = opTypeMapInitializer();

// ���������ַ���ӳ��
map<FuncUnitType, string> funcUnitTypeStr;

void initializeFuncUnitTypeStr() {
    funcUnitTypeStr[INT] = "����";
    funcUnitTypeStr[ADD] = "�ӷ�";
    funcUnitTypeStr[DIV] = "����";
    funcUnitTypeStr[MULT1] = "�˷�1";
    funcUnitTypeStr[MULT2] = "�˷�2";
}

// ��ʼ������
void init() {
    rob.entries.resize(MAX_INSTR + 1, -1); // ��ʼ��ROB��Ŀ
    initializeFuncUnitTypeStr(); // ��ʼ�����ܲ��������ַ���ӳ��
}

// ����ָ���
void read() {
    scanf("%d\n", &t); // ����ָ������

    for (int i = 1; i <= t; ++i) {
        string s;
        getline(cin, s); // ����һ��ָ��

        instructions[i].name = s; // ��¼ָ������

        // ����ָ�����ͺͲ�����
        if (s[0] == 'L') {
            instructions[i].op = "LD";
            sscanf(s.c_str(), "LD F%d,%*d(R%d)", &instructions[i].fi, &instructions[i].fj);
        } else {
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instructions[i].fi, &instructions[i].fj, &instructions[i].fk);
            if (s[0] == 'S') instructions[i].op = "SUBD";
            else if (s[0] == 'A') instructions[i].op = "ADDD";
            else if (s[0] == 'D') instructions[i].op = "DIVD";
            else if (s[0] == 'M') instructions[i].op = "MULTD";
        }
    }
}

// ��ӡ״̬����
void print(int cycle) {
    printf("                                                ����%d:\n", cycle);
    printf("ָ��״̬:\n");
    printf("ָ����        |Issue|Read operand|Execution completed|Write Result|\n");
    for (int i = 1; i <= t; ++i) {
        printf("%-14s|", instructions[i].name.c_str());
        if (instructions[i].issue != 0) printf("%5d|", instructions[i].issue);
        else printf("     |");
        if (instructions[i].read != 0) printf("%12d|", instructions[i].read);
        else printf("            |");
        if (instructions[i].execution != 0) printf("%19d|", instructions[i].execution);
        else printf("                   |");
        if (instructions[i].write != 0) printf("%12d|", instructions[i].write);
        else printf("            |");
        printf("\n");
    }
    printf("\n");
    printf("���ܲ���״̬:\n");
    printf("���ܲ�����|Busy |Op   |Fi |Fj |Fk |Qj   |Qk   |Rj |Rk |\n");
    for (int i = 0; i < 5; ++i) {
        printf("%-10s|", funcUnitTypeStr[reservationStations[i].type].c_str());
        if (reservationStations[i].busy) printf("true |");
        else printf("false|     |   |   |   |     |     |   |   |\n");
    }
    printf("\n");
    printf("ROB״̬:\n");
    printf("ROB��Ŀ|ָ����        |Issue|Read operand|Execution completed|Write Result|\n");
    for (int i = rob.head; i < rob.tail; ++i) {
        int idx = rob.entries[i];
        printf("%-7d|", i);
        if (idx != -1) {
            printf("%-14s|", instructions[idx].name.c_str());
            if (instructions[idx].issue != 0) printf("%5d|", instructions[idx].issue);
            else printf("     |");
            if (instructions[idx].read != 0) printf("%12d|", instructions[idx].read);
            else printf("            |");
            if (instructions[idx].execution != 0) printf("%19d|", instructions[idx].execution);
            else printf("                   |");
            if (instructions[idx].write != 0) printf("%12d|", instructions[idx].write);
            else printf("            |");
        }
        printf("\n");
    }
    printf("\n");
}

// Tomasulo�㷨����
void tomasulo() {
    queue<int> instr_queue; // ָ�����
    int cycle = 0; // ���ڼ�����

    for (int i = 1; i <= t; ++i) instr_queue.push(i); // ������ָ�����

    while (!instr_queue.empty() || rob.head < rob.tail) {
        cycle++;

        // ����׶�
        for (int i = 0; i < 5; ++i) {
            if (!reservationStations[i].busy && !instr_queue.empty()) {
                int idx = instr_queue.front();
                instr_queue.pop();
                instructions[idx].issue = cycle;
                reservationStations[i].busy = true;
                reservationStations[i].type = opTypeMap[instructions[idx].op];
                reservationStations[i].op = i;
                reservationStations[i].vj = instructions[idx].fj;
                reservationStations[i].vk = instructions[idx].fk;
                reservationStations[i].qj = reservationStations[i].qk = 0;
                reservationStations[i].dest = instructions[idx].fi;
                reservationStations[i].instruction_idx = idx;
            }
        }

        // ִ�н׶�
        for (int i = 0; i < 5; ++i) {
            if (reservationStations[i].busy) {
                int idx = reservationStations[i].instruction_idx;
                if (reservationStations[i].qj == 0 && reservationStations[i].qk == 0) {
                    reservationStations[i].busy = false;
                    instructions[idx].execution = cycle;
                    instructions[idx].write = cycle + 1;
                    instructions[idx].rob_done = true;
                }
            }
        }

        // ���۽׶�
        while (rob.head < rob.tail && instructions[rob.entries[rob.head]].rob_done) rob.head++;

        // ��ӡ״̬
        print(cycle);
    }
}

// ������
int main() {
    init(); // ��ʼ��

    read(); // ����ָ��

    tomasulo(); // ����Tomasulo�㷨

    return 0;
}

