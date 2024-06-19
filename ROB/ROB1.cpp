#include <stdio.h>
#include <stdbool.h>

#define N 6 // Reservation Stations
#define M 6 // Reorder Buffer
#define MEM_SIZE 100 // 假设内存大小为100

// Define Reservation Station and Reorder Buffer structs
typedef struct ReservationStation {
    bool Busy;
    int Dest;
    char Op;
    int Vj;
    int Vk;
    int Qj;
    int Qk;
    int A; // for store and load instructions
} ReservationStation;

typedef struct ReorderBuffer {
	char Busy;
    char Instruction;
    int Dest;
    bool Ready;
    int Value;
    int Address; // 添加 Address 成员
} ReorderBuffer;

ReservationStation RS[N];
ReorderBuffer ROB[M];
int Regs[16]; // Register File
struct {
    bool Busy;
    int Reorder;
} RegStat[16]; // Register Status
int Mem[MEM_SIZE]; // 定义内存数组

// Helper function prototypes
bool anyEarlierStoreInstructionInQueue(int r);
bool allPreviousStoresHaveDifferentAddresses();
int readMemory(int addr);
bool storeInstructionAtQueueHead(int r);
bool instructionCompleted(int r);
bool CDBReady();
bool instructionAtROBHeadReady();
bool branchMispredicted();
void clearROBAndRegisterStat(int h);
void fetchBranchDest();
bool loadStep1Completed(int r);

// Issue stage function
void issue(char Op, char Instruction, int rd, int rs, int rt, int imm) {
    int r, b, h;
    // Find free Reservation Station and Reorder Buffer entry
    for (r = 0; r < N; r++) {
        if (!RS[r].Busy) {
            RS[r].Busy = true;
            break;
        }
    }
    for (b = 0; b < M; b++) {
        if (!ROB[b].Instruction) {
            ROB[b].Instruction = Instruction;
            ROB[b].Dest = rd;
            ROB[b].Ready = false;
            break;
        }
    }
    RS[r].Dest = b;
    RS[r].Op = Op;
    // Check register status
    if (RegStat[rs].Busy) {
        h = RegStat[rs].Reorder;
        if (ROB[h].Ready) {
            RS[r].Vj = ROB[h].Value;
            RS[r].Qj = 0;
        } else {
            RS[r].Qj = h;
        }
    } else {
        RS[r].Vj = Regs[rs];
        RS[r].Qj = 0;
    }
    // For floating point operation and store
    if (Op == 'F' || Op == 'S') {
        if (RegStat[rt].Busy) {
            h = RegStat[rt].Reorder;
            if (ROB[h].Ready) {
                RS[r].Vk = ROB[h].Value;
                RS[r].Qk = 0;
            } else {
                RS[r].Qk = h;
            }
        } else {
            RS[r].Vk = Regs[rt];
            RS[r].Qk = 0;
        }
    }
    // For store and load
    if (Op == 'S' || Op == 'L') {
        RS[r].A = imm;
    }
    // Update register status
    if (Op == 'F' || Op == 'L') {
        RegStat[rd].Reorder = b;
        RegStat[rd].Busy = true;
    }
    if (Op == 'L') {
        RegStat[rt].Reorder = b;
        RegStat[rt].Busy = true;
    }
}

// Helper function implementations
bool anyEarlierStoreInstructionInQueue(int r) {
    for (int i = 0; i < r; i++) {
        if (RS[i].Op == 'S') {
            return true;
        }
    }
    return false;
}

bool allPreviousStoresHaveDifferentAddresses() {
    // Assume it always returns true for simplicity
    return true;
}

int readMemory(int addr) {
    // Assume it always returns a value for simplicity
    return Mem[addr];
}

bool storeInstructionAtQueueHead(int r) {
    if (RS[r].Op == 'S') {
        for (int i = 0; i < N; i++) {
            if (RS[i].Op == 'S' && i < r) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool instructionCompleted(int r) {
    return RS[r].Busy && ROB[RS[r].Dest].Ready;
}

bool CDBReady() {
    // Assume CDB is always ready for simplicity
    return true;
}

bool instructionAtROBHeadReady() {
    return ROB[0].Ready;
}

bool branchMispredicted() {
    // Assume branch is never mispredicted for simplicity
    return false;
}

void clearROBAndRegisterStat(int h) {
    for (int i = 0; i < M; i++) {
        ROB[i].Instruction = ' ';
        ROB[i].Dest = 0;
        ROB[i].Ready = false;
        ROB[i].Value = 0;
    }
    for (int i = 0; i < 16; i++) {
        RegStat[i].Busy = false;
        RegStat[i].Reorder = 0;
    }
}

void fetchBranchDest() {
    // Assume fetching branch destination for simplicity
}

bool loadStep1Completed(int r) {
    // Assume load step 1 is always completed for simplicity
    return true;
}

int main() {
    // Reset Reservation Stations, Reorder Buffer, and Register Status
    for (int i = 0; i < N; i++) {
        RS[i].Busy = false;
        RS[i].Dest = 0;
        RS[i].Op = ' ';
        RS[i].Vj = 0;
        RS[i].Vk = 0;
        RS[i].Qj = 0;
        RS[i].Qk = 0;
        RS[i].A = 0;
    }
    for (int i = 0; i < M; i++) {
        ROB[i].Instruction = ' ';
        ROB[i].Dest = 0;
        ROB[i].Ready = false;
        ROB[i].Value = 0;
        ROB[i].Address = 0;
    }
    for (int i = 0; i < 16; i++) {
        Regs[i] = 0;
        RegStat[i].Busy = false;
        RegStat[i].Reorder = 0;
    }

    // Input instructions
    issue('L', 'D', 6, 2, 0, 34);
    issue('L', 'D', 2, 3, 0, 45);
    issue('M', 'D', 0, 2, 4, 0);
    issue('S', 'D', 8, 6, 0, 0);
    issue('D', 'D', 10, 0, 6, 0);
    issue('D', 'D', 6, 8, 2, 0);

    // Execution stage
    for (int r = 0; r < N; r++) {
        if (RS[r].Op == 'F') {
            if (RS[r].Qj == 0 && RS[r].Qk == 0) {
                RS[r].Vk = RS[r].Vj + RS[r].Vk;
            }
        }
        if (RS[r].Op == 'L') {
            if (RS[r].Qj == 0 && !anyEarlierStoreInstructionInQueue(r)) {
                RS[r].A += RS[r].Vj;
            }
            if (loadStep1Completed(r) && allPreviousStoresHaveDifferentAddresses()) {
                Mem[RS[r].A] = readMemory(RS[r].A);
            }
        }
        if (RS[r].Op == 'S') {
            if (RS[r].Qj == 0 && storeInstructionAtQueueHead(r)) {
                ROB[RS[r].Dest].Address = RS[r].Vj + RS[r].A;
            }
        }
    }

    // Write result stage
    for (int r = 0; r < N; r++) {
        if (instructionCompleted(r) && CDBReady()) {
            int b = RS[r].Dest;
            RS[r].Busy = false;
            for (int x = 0; x < N; x++) {
                if (RS[x].Qj == b) {
                    RS[x].Vj = ROB[b].Value;
                    RS[x].Qj = 0;
                }
                if (RS[x].Qk == b) {
                    RS[x].Vk = ROB[b].Value;
                    RS[x].Qk = 0;
                }
            }
            ROB[b].Value = ROB[b].Address; // For load and store instructions
            ROB[b].Ready = true;
        }
    }

    // Confirmation stage
    for (int h = 0; h < M; h++) {
        if (instructionAtROBHeadReady()) {
            int d = ROB[h].Dest;
            if (ROB[h].Instruction == 'B') {
                if (branchMispredicted()) {
                    clearROBAndRegisterStat(h);
                    fetchBranchDest();
                }
            } else if (ROB[h].Instruction == 'S') {
                Mem[ROB[h].Address] = ROB[h].Value;
            } else { // For other instructions
                Regs[d] = ROB[h].Value;
            }
            ROB[h].Busy = false;
            if (RegStat[d].Reorder == h) {
                RegStat[d].Busy = false;
            }
        }
    }

    // Print the status of Reservation Stations and Reorder Buffer
    printf("Reservation Stations:\n");
    for (int i = 0; i < N; i++) {
        printf("RS[%d]: Busy=%d, Dest=%d, Op=%c, Vj=%d, Vk=%d, Qj=%d, Qk=%d, A=%d\n", 
               i, RS[i].Busy, RS[i].Dest, RS[i].Op, RS[i].Vj, RS[i].Vk, RS[i].Qj, RS[i].Qk, RS[i].A);
    }
    printf("\nReorder Buffer:\n");
    for (int i = 0; i < M; i++) {
        printf("ROB[%d]: Instruction=%c, Dest=%d, Ready=%d, Value=%d, Address=%d\n", 
               i, ROB[i].Instruction, ROB[i].Dest, ROB[i].Ready, ROB[i].Value, ROB[i].Address);
    }

    return 0;
}


