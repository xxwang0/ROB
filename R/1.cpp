#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

// ����һЩ���ݽṹ��ȫ�ֱ���
struct ROBEntry {
    string instruction;
    int dest;
    float value;
    bool ready;
    bool busy;
    int address; // ���ڴ洢ָ��ĵ�ַ
};

struct RSEntry {
    float Vj, Vk;
    int Qj, Qk;
    bool busy;
    int Dest;
    int A;
};

struct RegisterStatus {
    bool busy;
    int reorder;
};

// ʾ�����ݽṹ����
vector<ROBEntry> ROB;
vector<RSEntry> RS;
vector<float> Regs;
vector<RegisterStatus> RegStat;
vector<float> Mem;
queue<int> q; // ���д洢�������ָ��
queue<pair<int, int> > now; // ִ�ж���
string op;
int rs, rt, imm;

int find_free_RS() {
    for (int i = 0; i < RS.size(); ++i) {
        if (!RS[i].busy) return i;
    }
    return -1;
}

int find_free_ROB() {
    for (int i = 0; i < ROB.size(); ++i) {
        if (!ROB[i].busy) return i;
    }
    return -1;
}

void issue(int time) {
    if (!q.empty()) {
        int x = q.front();    
        int r = find_free_RS(); // �ҵ�һ�����еı���վ
        int b = find_free_ROB(); // �ҵ�һ�����е�ROB��
        if (r == -1 || b == -1) {
            cout << "No free reservation station or ROB entry available.\n";
            return;
        }
        // ���Դ�Ĵ�����״̬
        if (RegStat[rs].busy) {
            int h = RegStat[rs].reorder;
            if (ROB[h].ready) {
                RS[r].Vj = ROB[h].value; // ���ROBͷָ�������ȡ��
                RS[r].Qj = 0;
            } else {
                RS[r].Qj = h; // ���򣬵ȴ�ָ��
            }
        } 
        else {
            RS[r].Vj = Regs[rs]; // ���Ĵ������У�ֱ��ȡ��
            RS[r].Qj = 0;
        }

        RS[r].busy = true;
        RS[r].Dest = b;
        ROB[b].instruction = op;
        ROB[b].dest = rt;
        ROB[b].ready = false;

        // ����Ǹ��������洢����
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV" || op == "STORE") {
            if (RegStat[rt].busy) {
                int h = RegStat[rt].reorder;
                if (ROB[h].ready) {
                    RS[r].Vk = ROB[h].value; // ���ROBͷָ�������ȡ��
                    RS[r].Qk = 0;
                } else {
                    RS[r].Qk = h; // ���򣬵ȴ�ָ��
                }
            } else {
                RS[r].Vk = Regs[rt]; // ���Ĵ������У�ֱ��ȡ��
                RS[r].Qk = 0;
            }
        }

        // ���ڸ������
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
            RegStat[rt].reorder = b;
            RegStat[rt].busy = true;
            ROB[b].dest = rt;
        }

        // ����store����
        if (op == "STORE") {
            RS[r].A = imm;
        }

        // ����load����
        if (op == "LOAD") {
            RS[r].A = imm;
            RegStat[rt].reorder = b;
            RegStat[rt].busy = true;
            ROB[b].dest = rt;
        }

        // ������׶���ɵ� x ����ִ�ж���
        now.push(make_pair(x, 2));
        q.pop(); // ������������Ƴ��ѷ��������
    }
}

void execute(int time) {
    while (!now.empty()) {
        auto task = now.front();
        int x = task.first;
        now.pop();

        int r = x; // ���� x �Ǳ���վ����
        if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
            if (RS[r].Qj == 0 && RS[r].Qk == 0) {
                // ������
                float result = RS[r].Vj + RS[r].Vk; // ����Ϊ�ӷ�����
                // ������洢�� ROB ��
                ROB[RS[r].Dest].value = result;
                ROB[RS[r].Dest].ready = true;
            }
        } else if (op == "LOAD") {
            if (RS[r].Qj == 0) {
                RS[r].A = RS[r].Vj + RS[r].A;
                ROB[RS[r].Dest].value = Mem[RS[r].A];
                ROB[RS[r].Dest].ready = true;
            }
        } else if (op == "STORE") {
            if (RS[r].Qj == 0) {
                ROB[RS[r].Dest].address = RS[r].Vj + RS[r].A;
                // �����洢��ز���
            }
        }
    }
}

// ����ִ�н��д�ؽ׶εĺ���
void execute_write_result(int r, float result) {
    int b = RS[r].Dest; // ȡ��Ŀ�� ROB ��Ŀ����

    RS[r].busy = false; // ����ǰ����վ��Ϊ��æ״̬

    // �������������ڵ�ǰָ�����ı���վ��״̬
    for (int x = 0; x < RS.size(); ++x) {
        if (RS[x].Qj == b) {
            RS[x].Vj = result; // �����д�� Vj
            RS[x].Qj = 0; // ��� Qj
        }
        if (RS[x].Qk == b) {
            RS[x].Vk = result; // �����д�� Vk
            RS[x].Qk = 0; // ��� Qk
        }
    }

    // �����д�� ROB
    ROB[b].value = result;
    ROB[b].ready = true; // ���Ϊ����״̬
}

// �����ύ�׶εĺ���ʵ��
void commit() {
    // ȷ��Ҫ�����ָ��λ�� ROB ��ͷ�����Ѿ���
    if (!ROB.empty() && ROB[0].ready) {
        int h = 0; // ROB ͷ������
        int d = ROB[h].dest; // Ŀ��Ĵ�������

        if (ROB[h].instruction == "STORE") {
            // ����洢ָ��
            Mem[ROB[h].address] = ROB[h].value; // �洢���ڴ�
        } else {
            // ��������ָ������д��Ŀ��Ĵ���
            Regs[d] = ROB[h].value;
        }

        // �ͷ� ROB ��Ŀ
        ROB[h].busy = false;

        // ���û������ָ������д��Ŀ��Ĵ��������ͷ�Ŀ��Ĵ���
        if (RegStat[d].reorder == h) {
            RegStat[d].busy = false;
        }

        // �Ƴ� ROB ͷ����Ŀ
        ROB.erase(ROB.begin());
    }
}

int main() {
    // ��ʼ�����ݽṹ�Ͷ��е�
    // ���� RS��ROB��Regs��RegStat��Mem �Ĵ�С��ʼ��
    RS.resize(10);
    ROB.resize(10);
    Regs.resize(32);
    RegStat.resize(32);
    Mem.resize(1024);

    // ʾ���������
    q.push(0);
    q.push(1);
    q.push(2);

    // ����ȫ�ֱ���ʾ��
    op = "ADD";
    rs = 1;
    rt = 2;
    imm = 10;

    // ʾ��ʱ������ģ��
    int time = 0;
    while (!q.empty() || !now.empty()) {
        issue(time);
        execute(time);
        commit();
        time++;
    }

    return 0;
}

