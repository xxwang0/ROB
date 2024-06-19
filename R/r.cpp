#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

struct Instruction {
    std::string op;  // ���������� ADD, SUB, MUL, DIV
    int dest;  // Ŀ�ļĴ���
    int src1;  // Դ�Ĵ���1
    int src2;  // Դ�Ĵ���2
};

struct ReservationStation {
    bool busy;
    std::string op;
    int Vj, Vk;
    int Qj, Qk;
    int dest;
    int A;  // ��ַ
};

struct ROBEntry {
    bool busy;
    std::string state;  // "issue", "execute", "writeback", "commit"
    int dest;  // Ŀ�ļĴ���
    int value;  // ֵ
    bool ready;  // �Ƿ��Ѽ������
};

class Tomasulo {
public:
    Tomasulo(int numRS, int numROB) 
        : reservationStations(numRS), rob(numROB), head(0), tail(0) {}

    void issue(Instruction& instr);
    void execute();
    void writeback();
    void commit();
    
private:
    std::vector<ReservationStation> reservationStations;
    std::vector<ROBEntry> rob;
    std::unordered_map<int, int> registerFile;
    std::unordered_map<int, int> registerStatus;
    int head, tail;
};
void Tomasulo::issue(Instruction& instr) {
    // Ѱ�ҿ��еı���վ
    for (auto& rs : reservationStations) {
        if (!rs.busy) {
            rs.busy = true;
            rs.op = instr.op;
            rs.dest = instr.dest;
            
            // Դ������1
            if (registerStatus[instr.src1] != -1) {
                rs.Qj = registerStatus[instr.src1];
            } else {
                rs.Qj = -1;
                rs.Vj = registerFile[instr.src1];
            }
            
            // Դ������2
            if (registerStatus[instr.src2] != -1) {
                rs.Qk = registerStatus[instr.src2];
            } else {
                rs.Qk = -1;
                rs.Vk = registerFile[instr.src2];
            }
            
            // ��ӵ�ROB
            rob[tail].busy = true;
            rob[tail].state = "issue";
            rob[tail].dest = instr.dest;
            rob[tail].ready = false;
            
            registerStatus[instr.dest] = tail;
            tail = (tail + 1) % rob.size();
            break;
        }
    }
}
void Tomasulo::execute() {
    for (auto& rs : reservationStations) {
        if (rs.busy && rs.Qj == -1 && rs.Qk == -1) {
            // ִ��ָ��
            if (rs.op == "ADD") {
                rs.A = rs.Vj + rs.Vk;
            } else if (rs.op == "SUB") {
                rs.A = rs.Vj - rs.Vk;
            } // �������������
            
            // ���Ϊ�Ѽ������
            for (auto& entry : rob) {
                if (entry.busy && entry.dest == rs.dest) {
                    entry.value = rs.A;
                    entry.ready = true;
                    entry.state = "execute";
                    break;
                }
            }
            
            rs.busy = false;
        }
    }
}
void Tomasulo::writeback() {
    for (auto& entry : rob) {
        if (entry.busy && entry.ready && entry.state == "execute") {
            registerFile[entry.dest] = entry.value;
            entry.state = "writeback";
            
            // ���±���վ
            for (auto& rs : reservationStations) {
                if (rs.Qj == &entry - &rob[0]) {
                    rs.Qj = -1;
                    rs.Vj = entry.value;
                }
                if (rs.Qk == &entry - &rob[0]) {
                    rs.Qk = -1;
                    rs.Vk = entry.value;
                }
            }
        }
    }
}
void Tomasulo::commit() {
    if (rob[head].busy && rob[head].state == "writeback") {
        registerFile[rob[head].dest] = rob[head].value;
        registerStatus[rob[head].dest] = -1;
        
        rob[head].busy = false;
        head = (head + 1) % rob.size();
    }
}
int main() {
    Tomasulo tomasulo(5, 10);  // 5������վ��10��ROB��Ŀ
    
    std::vector<Instruction> instructions = {
        {"ADD", 1, 2, 3},
        {"SUB", 4, 1, 5},
        // ��Ӹ���ָ��
    };
    
    for (auto& instr : instructions) {
        tomasulo.issue(instr);
        tomasulo.execute();
        tomasulo.writeback();
        tomasulo.commit();
    }
    
    return 0;
}

