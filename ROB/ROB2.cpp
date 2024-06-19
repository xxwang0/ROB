#include <cstdio>
#include <algorithm>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <windows.h>
#include <cstdlib>
int t;

std::queue<int> q; // ָ����У�������У�
struct st
{
    int n, step;
};
struct instr // ���ָ��ĸ���ϸ��
{
    std::string op;
    int fi, fj, fk, cycle, device, rj, rk, qj, qk;
    int issue, read, execution, write;
    std::string name;
} instruction[100 + 1];

std::queue<int> reg_q[30 + 1]; // �����Ҫ�üĴ��������ָ��

int reg[31 + 1];    // ���д�üĴ�����ָ��
int device[5 + 1];   // ���ռ�øù��ܲ�����ָ��
std::set<int> step[6]; // ��Ÿ����׶ε�ָ�����
std::set<int>::iterator it, ite; // ������������ö��
std::map<std::string, int> map; // ������ӳ�䣬�����������ָ��ָ����ϵ
std::map<int, std::string> mapp; // ������ӳ�䣬�����������ָ��ָ����ϵ
std::string m[3 + 1][30 + 1]; // ������ӳ�䣬�����������ָ��ָ����ϵ

// ��ʼ��
inline void init()
{
    map["LD"] = 1;
    map["SUBD"] = 2;
    map["ADDD"] = 2;
    map["DIVD"] = 3;
    map["MULTD"] = 4;
    mapp[1] = "����";
    mapp[2] = "�ӷ�";
    mapp[3] = "����";
    mapp[4] = "�˷�1";
    mapp[5] = "�˷�2";
    m[1][1] = "��";
    m[2][1] = "��";
    m[3][1] = "  ";
    m[1][2] = "��";
    m[2][2] = "��";
    m[3][2] = "  ";
    m[1][3] = "��";
    m[2][3] = "��";
    m[3][3] = "  ";
    m[1][4] = "��";
    m[2][4] = "��";
    m[3][4] = "1 ";
    m[1][5] = "��";
    m[2][5] = "��";
    m[3][5] = "2 ";
}

// ��ָ��
inline void read()
{
    // ����Ҫִ�е�ָ����
    scanf("%d", &t);
    // ȥ�����໻�з�����ֹ�������
    char ch;
    while (1)
    {
        ch = getchar();
        if (ch == '\n')
            break;
    }

    for (int i = 1; i <= t; i++)
    {
        std::string s;
        getline(std::cin, s);
        instruction[i].name = s;

        // ����ָ�����ָ�����ͳ�ʼ��ָ��ṹ����ֶ�
        if (s.substr(0, 2) == "LD")
        {
            instruction[i].op = "LD";
            instruction[i].cycle = 1;

            // ����Ŀ�ļĴ�����Դ������
            sscanf(s.c_str(), "%*s %*s F%d,%*d(R%d)", &instruction[i].fi, &instruction[i].fj);
        }
        else if (s.substr(0, 2) == "MU")
        {
            instruction[i].op = "MULTD";
            instruction[i].cycle = 10;

            // ����Ŀ�ļĴ�����Դ������
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
        else if (s.substr(0, 2) == "SU")
        {
            instruction[i].op = "SUBD";
            instruction[i].cycle = 2;

            // ����Ŀ�ļĴ�����Դ������
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
        else if (s.substr(0, 2) == "DI")
        {
            instruction[i].op = "DIVD";
            instruction[i].cycle = 40;

            // ����Ŀ�ļĴ�����Դ������
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
        else if (s.substr(0, 2) == "AD")
        {
            instruction[i].op = "ADDD";
            instruction[i].cycle = 2;

            // ����Ŀ�ļĴ�����Դ������
            sscanf(s.c_str(), "%*s F%d,F%d,F%d", &instruction[i].fi, &instruction[i].fj, &instruction[i].fk);
        }
    }
// ִ�н׶�
// ö��ִ�ж���
for (it = step[3].begin(); it != step[3].end(); ++it)
{
    int x = *it;
    // ����ָ�����ͽ�����Ӧ�Ĵ���
    if (instruction[x].op == "LD")
    {
        // LD ָ��ִ����ɺ󣬽�Ŀ�ļĴ����е�ֵ����Ϊ���ڴ��м��ص�ֵ
        int cycle;
        instruction[x].execution = cycle;
        // ģ����ڴ��м������ݵĹ���
        // ������ڴ��ַ Mem[instruction[x].fj] �м������ݵ��Ĵ��� instruction[x].fi
        reg[instruction[x].fi] = Mem[instruction[x].fj];
        // ��ִ�н׶���ɵ�ָ�� x ����д������У�����ִ�ж���ɾ��
        now.push((st){x, 4});
    }
    else if (instruction[x].op == "ADDD" || instruction[x].op == "SUBD" || instruction[x].op == "DIVD" || instruction[x].op == "MULTD")
    {
        // ��������ָ���ִ���߼�
        // ʡ�Բ��ִ��룬������ľ�������ʵ��
    }
}
// д����׶�
// ö��д�������
for (it = step[4].begin(); it != step[4].end(); ++it)
{
    int x = *it;
    // ����ָ�����ͽ�����Ӧ�Ĵ���
    if (instruction[x].op == "LD")
    {
        // LD ָ���Ҫִ��д����׶Σ�ֱ������
        continue;
    }
    else if (instruction[x].op == "ADDD" || instruction[x].op == "SUBD" || instruction[x].op == "DIVD" || instruction[x].op == "MULTD")
    {
        // ����ָ��Ľ�����������д�����Ĵ������ߴ洢����
        // ������Ҫ������ľ���������ʵ�֣����罫�������������Ĵ������ߴ洢��
        // Ȼ����� ROB �е���Ϣ
        instruction[x].write = time;
        ROB[instruction[x].Dest].Value = instruction[x].Result; // ���� ROB �����ݽṹ���� Value �� Dest �ֶ�
        ROB[instruction[x].Dest].Ready = true;
        // ��д����׶���ɵ�ָ�� x ����ȷ�϶��У�����д�������ɾ��
        now.push((st){x, 5});
    }
}
// ȷ�Ͻ׶�
// ö��ȷ�϶���
for (it = step[5].begin(); it != step[5].end(); ++it)
{
    int h = *it; // ROB ͷ������
    std::string instruction = ROB[h].Instruction;

    // ���ݲ�ָͬ������ִ����Ӧ��ȷ�ϲ���
    if (instruction == "Branch")
    {
        // ��ָ֧���ȷ���߼�
        if (branchMispredicted()) // �����֧Ԥ�����
        {
            clearROBEntry(h);
            clearRegisterStatus(); // ��ռĴ���״̬
            fetchBranchDestination(); // ��ȡ��֧Ŀ���ַ
        }
    }
    else if (instruction == "Store")
    {
        // �洢ָ���ȷ���߼�
        Mem[ROB[h].Dest] = ROB[h].Value; // ���� Mem �Ǵ洢������
    }
    else
    {
        // ��������ָ���ȷ���߼�����������д��Ŀ��Ĵ���
        Regs[ROB[h].Dest] = ROB[h].Value; // ���� Regs �ǼĴ�������
    }

    ROB[h].Busy = false; // �ͷ� ROB ��Ŀ

    // ���Ŀ��Ĵ���û������ָ������д�룬���ͷŸüĴ���
    if (RegStat[ROB[h].Dest].Reorder == h)
    {
        RegStat[ROB[h].Dest].Busy = false;
    }
}
// ��֧Ԥ������麯��
bool branchMispredicted()
{
    // �����Ƿ�֧Ԥ�������߼��ж�
    // ���� true ��ʾ��֧Ԥ����󣬷��򷵻� false
    // ������� mispredicted() ��һ�������жϷ�֧Ԥ���Ƿ����ĺ���
    return mispredicted();
}

// ��� ROB ��Ŀ����
void clearROBEntry(int index)
{
    // ��������� ROB ��Ŀ���߼�
    // ���ָ�������� ROB ��Ŀ
    ROB[index].Value = 0;
    ROB[index].Ready = false;
    // ������������ֶ�...
}

// ��ռĴ���״̬����
void clearRegisterStatus()
{
    // ��������ռĴ���״̬���߼�
    // ������мĴ�����״̬��Ϣ����æ��״̬��
    for (int i = 0; i < NumRegisters; ++i)
    {
        RegStat[i].Busy = false;
        // ������������ֶ�...
    }
}

// ��ȡ��֧Ŀ���ַ����
void fetchBranchDestination()
{
    // �����ǻ�ȡ��֧Ŀ���ַ���߼�
    // ��ȡ��֧Ԥ�����ʱ��Ŀ���ַ������ PC ����Ϊ�õ�ַ
    PC = mispredictedBranchTarget();
}
// ���д�ؽ׶Σ�ȷ�Ͻ׶Σ�
void writeBackStage()
{
    // ���Ҫ�����ָ��λ�� ROB ��ͷ����ָ����׼������
    if (ROBHeadReady())
    {
        // ��ȡĿ��Ĵ������
        int destReg = ROBHeadDestination();

        // ��ָ֧�����⴦��
        if (ROBHeadInstruction() == Branch)
        {
            // ����Ƿ��֧Ԥ�����
            if (branchMispredicted())
            {
                // ��� ROB ��Ŀ�ͼĴ���״̬
                clearROBEntry(ROBHeadIndex());
                clearRegisterStatus();

                // ��ȡ��֧Ŀ���ַ������ PC
                fetchBranchDestination();
            }
        }
        else if (ROBHeadInstruction() == Store)
        {
            // �洢ָ��д���ڴ�
            Mem[ROBHeadDestination()] = ROBHeadValue();
        }
        else
        {
            // �����д��Ŀ��Ĵ���
            Regs[destReg] = ROBHeadValue();

            // ��� ROB ��ĿΪ����
            ROB[ROBHeadIndex()].Busy = false;

            // ���Ŀ��Ĵ���״̬���ڵ�ǰ ROB ͷ�����������ͷ�Ŀ��Ĵ���
            if (RegStat[destReg].Reorder == ROBHeadIndex())
            {
                RegStat[destReg].Busy = false;
            }
        }
    }
}
// ������
int main()
{
    // ��ʼ��
    init();

    // ��ȡָ��
    readInstructions();

    // ִ�� Tomasulo �㷨
    executeTomasulo();

    return 0;
}

