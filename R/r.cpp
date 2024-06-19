#include <vector>
#include <queue>
#include <unordered_map>
#include <string>

struct Instruction {
    std::string op;  // 操作符，如 ADD, SUB, MUL, DIV
    int dest;  // 目的寄存器
    int src1;  // 源寄存器1
    int src2;  // 源寄存器2
};

struct ReservationStation {
    bool busy;
    std::string op;
    int Vj, Vk;
    int Qj, Qk;
    int dest;
    int A;  // 地址
};

struct ROBEntry {
    bool busy;
    std::string state;  // "issue", "execute", "writeback", "commit"
    int dest;  // 目的寄存器
    int value;  // 值
    bool ready;  // 是否已计算完成
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
    // 寻找空闲的保留站
    for (auto& rs : reservationStations) {
        if (!rs.busy) {
            rs.busy = true;
            rs.op = instr.op;
            rs.dest = instr.dest;
            
            // 源操作数1
            if (registerStatus[instr.src1] != -1) {
                rs.Qj = registerStatus[instr.src1];
            } else {
                rs.Qj = -1;
                rs.Vj = registerFile[instr.src1];
            }
            
            // 源操作数2
            if (registerStatus[instr.src2] != -1) {
                rs.Qk = registerStatus[instr.src2];
            } else {
                rs.Qk = -1;
                rs.Vk = registerFile[instr.src2];
            }
            
            // 添加到ROB
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
            // 执行指令
            if (rs.op == "ADD") {
                rs.A = rs.Vj + rs.Vk;
            } else if (rs.op == "SUB") {
                rs.A = rs.Vj - rs.Vk;
            } // 添加其他操作符
            
            // 标记为已计算完成
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
            
            // 更新保留站
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
    Tomasulo tomasulo(5, 10);  // 5个保留站，10个ROB条目
    
    std::vector<Instruction> instructions = {
        {"ADD", 1, 2, 3},
        {"SUB", 4, 1, 5},
        // 添加更多指令
    };
    
    for (auto& instr : instructions) {
        tomasulo.issue(instr);
        tomasulo.execute();
        tomasulo.writeback();
        tomasulo.commit();
    }
    
    return 0;
}

