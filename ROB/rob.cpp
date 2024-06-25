//#include "sim_ooo.h"
// 包含自定义头文件 "sim_ooo.h"，用于声明和定义 sim_ooo 类

#include <iostream>
// 包含 C++ 标准库的输入输出流头文件

#include <stdlib.h>
// 包含 C 标准库的通用工具函数头文件

#include <sstream>
// 包含 C++ 标准库的字符串流头文件，用于字符串处理

#ifndef SIM_OO_H_
#define SIM_OO_H_
// 条件编译指令，确保头文件只被包含一次，避免重复定义

#include <stdio.h>
// 包含 C 标准库的输入输出函数头文件

#include <stdbool.h>
// 包含 C 标准库的布尔类型定义头文件

#include <string>
// 包含 C++ 标准库的字符串处理头文件

#include <sstream>
// 再次包含 C++ 标准库的字符串流头文件，用于字符串处理

#include <cstring>
// 包含 C 标准库的字符串处理头文件

using namespace std;
// 使用 C++ 标准库的命名空间 std，避免每次使用标准库中函数时都需要加上 std::

#define UNDEFINED 0xFFFFFFFF
// 定义常量 UNDEFINED，其值为 0xFFFFFFFF，用于初始化寄存器

#define NUM_GP_REGISTERS 32
// 定义常量 NUM_GP_REGISTERS，表示整数通用寄存器数量为 32

#define NUM_FP_REGISTERS 32
// 定义常量 NUM_FP_REGISTERS，表示浮点数寄存器数量为 32

#define NUM_OPCODES 28
// 定义常量 NUM_OPCODES，表示操作码数量为 28

#define NUM_STAGES 4
// 定义常量 NUM_STAGES，表示流水线阶段数量为 4

typedef enum {LW, SW, ADD, ADDI, SUB, SUBI, XOR, XORI, OR, ORI, AND, ANDI, MULT, DIV, BEQZ, BNEZ, BLTZ, BGTZ, BLEZ, BGEZ, JUMP, EOP, LWS, SWS, ADDS, SUBS, MULTS, DIVS} opcode_t;
// 定义枚举类型 opcode_t，包含处理器支持的指令操作码，共计 28 种

typedef enum {INTEGER_RS, ADD_RS, MULT_RS, LOAD_B} res_station_t;
// 定义枚举类型 res_station_t，包含预约站的类型，包括整数预约站、加法预约站、乘法预约站和加载缓冲区

typedef enum {INTEGER, ADDER, MULTIPLIER, DIVIDER, MEMORY, NOTVALID} exe_unit_t;
// 定义枚举类型 exe_unit_t，表示执行单元的类型，包括整数单元、加法器、乘法器、除法器、内存和无效类型

typedef enum{ISSUE, EXECUTE, WRITE_RESULT, COMMIT, INVALID} stage_t;
// 定义枚举类型 stage_t，表示流水线阶段的类型，包括发布、执行、写结果、提交和无效阶段

typedef enum{INTEGER_RES,FLOAT_RES,STORE_DEST,NOTASSIGNED} res_type_t;
// 定义枚举类型 res_type_t，表示结果的类型，包括整数结果、浮点数结果、存储目的地和未分配

class sim_ooo {
	/* Add the data members required by your simulator's implementation here */

	// 数据存储器 - 应初始化为全 0xFF
	unsigned char *data_memory;

	// 内存大小（字节）
	unsigned base_add;
	unsigned data_memory_size;
	unsigned register_file[NUM_GP_REGISTERS];
	float float_reg_file[NUM_FP_REGISTERS];
	unsigned current_PC;
	int head_ROB;
	unsigned current_B;
	unsigned current_R_int;
	unsigned current_R_mul;
	unsigned current_R_add;
	unsigned current_R_ld;
	unsigned check_branch;
	unsigned branch_cond;
	unsigned int_struc_hazard;
	unsigned load_done;
	unsigned add_struc_hazard;
	unsigned mul_struc_hazard;
	unsigned div_struc_hazard;
	unsigned issue_size;
	unsigned issue_success;
	unsigned num_cycles;
	unsigned load_complete;
	unsigned check_end;
	unsigned eop_end;
	unsigned num_instructions;
	unsigned is_cleared;
	unsigned is_store_ex;
	unsigned is_load_ex;
	unsigned if_branch;

public:

	
	sim_ooo(unsigned mem_size, 		// 数据存储器大小（字节）
		unsigned rob_size, 		// ROB 条目数量
                unsigned num_int_res_stations,	// 整数预约站数量
                unsigned num_add_res_stations,	// 加法预约站数量
                unsigned num_mul_res_stations, 	// 乘法/除法预约站数量
                unsigned num_load_buffers,	// 加载缓冲区数量
		unsigned issue_width=1		// 发布宽度

        );

	// 释放模拟器
	~sim_ooo();

        // 添加一个或多个指定类型的执行单元到处理器
        // - exec_unit: 要添加的执行单元类型
        // - latency: 执行单元的延迟（时钟周期数）
        // - instances: 要添加的该类型执行单元的数量
        void init_exec_unit(exe_unit_t exec_unit, unsigned latency, unsigned instances=1);

	// 在指定地址加载文件名为 "filename" 的汇编程序到指令内存中
	void load_program(const char *filename, unsigned base_address=0x0);

	// 运行模拟器，运行指定的时钟周期数 "cycles" （若 cycles=0，则运行至程序结束）
	void run(unsigned cycles=0);

	// Tomasulo 算法的发布阶段
	void issue(unsigned temp_PC);

	// Tomasulo 算法的执行阶段
	void execute_ins();

	// Tomasulo 算法的写结果阶段
	void write_res();

	// 提交阶段
	void commit_stage();

	// 重置模拟器状态
        /* 注意：
	   - 寄存器应重置为 UNDEFINED 值
	   - 数据存储器应重置为全 0xFF 值
	   - 指令窗口、预约站和 ROB 应清空
	*/
	void reset();

       // 返回指定整数通用寄存器的值
        int get_int_register(unsigned reg);

        // 将给定整数通用寄存器的值设置为 "value"
        void set_int_register(unsigned reg, int value);

        // 返回指定浮点数通用寄存器的值
        float get_fp_register(unsigned reg);

        // 将给定浮点数通用寄存器的值设置为 "value"
        void set_fp_register(unsigned reg, float value);

	// 返回将写入此整数寄存器的 ROB 条目索引（若寄存器值未挂起，则返回 UNDEFINED）
	unsigned get_pending_int_register(unsigned reg);

	// 返回将写入此浮点数寄存器的 ROB 条目索引（若寄存器值未挂起，则返回 UNDEFINED）
	unsigned get_pending_fp_register(unsigned reg);

	// 返回 IPC（指令每周期）
	float get_IPC();

	// 返回已完全执行的指令数量
	unsigned get_instructions_executed();

	// 返回时钟周期数
	unsigned get_clock_cycles();

	// 打印指定地址范围内的数据存储器内容
	void print_memory(unsigned start_address, unsigned end_address);

	// 在指定地址处写入整数值到数据存储器（使用小端格式：https://en.wikipedia.org/wiki/Endianness）
	void write_memory(unsigned address, unsigned value);

	// 打印寄存器的值
	void print_registers();

	// 打印处理器状态（不包括内存）
	void print_status();

	// 打印 ROB 的内容
	void print_rob();

	// 打印预约站的内容
	void print_reservation_stations();

	// 打印指令窗口的内容
	void print_pending_instructions();

	// 打印整个执行历史记录
	void print_log();
};

#endif /*SIM_OOO_H_*/

// #include "sim_ooo.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;
//存储每条指令的详细信息
struct every_line
{
	string branch_array;
	string op_code;
	string mand_operand;
	string remainder_operand1;
	string remainder_operand2;
	unsigned instr_address;
	exe_unit_t classify_exe;
};
// 记录了指令在不同阶段的时钟周期数，包括发射、执行、写回和提交阶段。
struct execution_log{
  unsigned log_pc;
  unsigned log_issue;
  unsigned log_execute;
  unsigned log_write;
  unsigned log_commit;
};

struct ROB_struc{
  unsigned long long entry;
  unsigned long long rob_busy;
  unsigned long long ready;
  unsigned long long rob_pc;
	unsigned long long rob_temp_pc;
	unsigned long long if_branch_rob;
  stage_t state;
  unsigned long long rob_dest;
  unsigned long long rob_val;
	res_type_t dest_type;
	unsigned long long if_commit;
	unsigned long long add_reg;
	unsigned long long str_add;
  unsigned long long is_available;
};
// 保留站的状态和信息
struct RES_Stat_struc{
  string res_name;
  unsigned long long res_busy;
  unsigned long long res_pc;
	 long long Vj;
   long long Vk;
  unsigned long long Qj;
  unsigned long long Qk;
  unsigned long long res_dest;
  unsigned long long res_A;
	unsigned long long op_num;
	unsigned long long if_branch;
	unsigned long long use_ex;
	unsigned long long target_address;
	unsigned long long if_depend_Vj;
	unsigned long long if_depend_Vk;
	unsigned long long wr_done;
	unsigned long long imm_val;
	unsigned long long address_raw;
	unsigned long long from_forward;
  unsigned long long execution_unit_used;
	res_type_t res_dest_type;
	exe_unit_t op;
};

struct REGISTER_Stat{
  unsigned ROB_num;
  unsigned reg_busy;
};

struct exe_struc{
  long long int Vj;
  long long int Vk;
  long long int Add;
	unsigned long long res_stat_num;
 	unsigned long long status;
};

struct pack_of_exec
{
	exe_unit_t exe_unit;
	unsigned latency_exe;
	unsigned instance_exe;
};

std::map<string, int> opcode_map;//存储字符串到整数的映射关系。将指令操作码（opcode）映射到其对应的数值

every_line every_instruct; //存储程序中每条指令的相关信息

ROB_struc ROB_tab = {1,0,0,UNDEFINED,UNDEFINED,UNDEFINED,INVALID,UNDEFINED,UNDEFINED,NOTASSIGNED,0,UNDEFINED,UNDEFINED,0};
RES_Stat_struc RES_Temp = {"",0,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,0,0,0,0,0,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,NOTASSIGNED,NOTVALID};
exe_struc TempReg = {UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,1};
pack_of_exec execution_structure;
execution_log temp_log = {UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED,UNDEFINED};
vector<REGISTER_Stat> int_reg_stat;
vector<REGISTER_Stat> float_reg_stat;

vector<execution_log> pending_ins;
vector<execution_log> final_log;
vector<pack_of_exec> create_exe_unit;

vector<every_line> instruction_memory;

vector<RES_Stat_struc> RES_Stat_Int;
vector<RES_Stat_struc> RES_Stat_Load;
vector<RES_Stat_struc> RES_Stat_Mul;
vector<RES_Stat_struc> RES_Stat_Add;
vector<int> int_result;
vector<int> add_result;
vector<int> mul_result;
vector<int> mem_result;
vector<int> flag_int;
vector<int> flag_add;
vector<int> flag_mem;
vector<int> flag_mul;
vector<int> int_ex_done;
vector<int> add_ex_done;
vector<int> mul_ex_done;
vector<int> div_ex_done;
vector<int> ld_ex_done;
vector<int> temp_res_add;
vector<int> needs_ex_ld;
vector<int> ld_step1;
vector<int> ld_step2;

vector<int> ld_current_exe;

vector<ROB_struc> ROB_table;

vector<exe_struc> big_int;
vector<exe_struc> big_add;
vector<exe_struc> big_mul;
vector<exe_struc> big_div;
vector<exe_struc> big_mem;

vector<int> num_latency_int;
vector<int> num_latency_add;
vector<int> num_latency_mul;
vector<int> num_latency_div;
vector<int> num_latency_mem;



static const char *stage_names[NUM_STAGES] = {"ISSUE", "EXE", "WR", "COMMIT"};
static const char *opcode_name[NUM_OPCODES] = {"LW", "SW", "ADD", "ADDI", "SUB", "SUBI", "XOR", "XORI", "OR", "ORI", "AND", "ANDI", "MULT", "DIV", "BEQZ", "BNEZ", "BLTZ", "BGTZ", "BLEZ", "BGEZ", "JUMP", "EOP", "LWS", "SWS", "ADDS", "SUBS", "MULTS", "DIVS"};
static const char *res_station_names[5]={"Int", "Add", "Mult", "Load"};

// 将浮点数转换为无符号整数
inline unsigned float2unsigned(float value){
        unsigned result;
        memcpy(&result, &value, sizeof value);
        return result;
}

// 无符号整数转换为浮点数
inline float unsigned2float(unsigned value){
        float result;
        memcpy(&result, &value, sizeof value);
        return result;
}

// 无符号整数与字符数组相互转换 
inline void unsigned2char(unsigned value, unsigned char *buffer){
    buffer[0] = value & 0xFF;         // 最低有效字节（最右边的字节）
    buffer[1] = (value >> 8) & 0xFF;  // 次低有效字节
    buffer[2] = (value >> 16) & 0xFF; // 次高有效字节
    buffer[3] = (value >> 24) & 0xFF; // 最高有效字节（最左边的字节）
}


inline unsigned char2unsigned(unsigned char *buffer){
       return buffer[0] + (buffer[1] << 8) + (buffer[2] << 16) + (buffer[3] << 24);
}

//构造函数，初始化 
sim_ooo::sim_ooo(unsigned mem_size,
                unsigned rob_size,
                unsigned num_int_res_stations,
                unsigned num_add_res_stations,
                unsigned num_mul_res_stations,
                unsigned num_load_res_stations,
		unsigned max_issue){
	//memory
	data_memory_size = mem_size;
	data_memory = new unsigned char[data_memory_size];
  //fill here
	issue_size = max_issue;

	for (unsigned a = 0; a < NUM_OPCODES; a++){
			opcode_map[opcode_name[a]] = a;
	}

  for(unsigned i = 0; i < NUM_GP_REGISTERS; i++){     
		register_file[i] = UNDEFINED;
	}

	for(unsigned i = 0; i < NUM_FP_REGISTERS; i++){   
		float_reg_file[i] = UNDEFINED;
	}

  for(unsigned j = 0; j < data_memory_size; j++){   
    data_memory[j] = 0xFF;
  }


  ROB_table.push_back(ROB_tab);
  pending_ins.push_back(temp_log);
  for(unsigned a = 1; a < rob_size; a++){           
    ROB_tab.entry = a+1;
    pending_ins.push_back(temp_log);
    ROB_table.push_back(ROB_tab);
  }

	RES_Temp.res_name = "Int";
  for(unsigned lint = 0; lint < num_int_res_stations; lint++){
    RES_Stat_Int.push_back(RES_Temp);
		int_result.push_back(UNDEFINED);
		flag_int.push_back(0);
  }

	RES_Temp.res_name = "Mult";
  for(unsigned lmul = 0; lmul < num_mul_res_stations; lmul++){
    RES_Stat_Mul.push_back(RES_Temp);
		mul_result.push_back(UNDEFINED);
		flag_mul.push_back(0);
  }

	RES_Temp.res_name = "Add";
  for(unsigned ladd = 0; ladd < num_add_res_stations; ladd++){
    RES_Stat_Add.push_back(RES_Temp);
		add_result.push_back(UNDEFINED);
		flag_add.push_back(0);
  }

	RES_Temp.res_name = "Load";
  for(unsigned lload = 0; lload < num_load_res_stations; lload++){
    RES_Stat_Load.push_back(RES_Temp);
		mem_result.push_back(UNDEFINED);
		flag_mem.push_back(0);
		needs_ex_ld.push_back(UNDEFINED);
		ld_step1.push_back(0);
    ld_step2.push_back(0);
		ld_current_exe.push_back(0);
		temp_res_add.push_back(UNDEFINED);
  }

  int_reg_stat.resize(NUM_GP_REGISTERS);
  float_reg_stat.resize(NUM_FP_REGISTERS);

  for(unsigned i = 0; i < NUM_GP_REGISTERS; i++){
    int_reg_stat[i].reg_busy = 0;
    int_reg_stat[i].ROB_num = UNDEFINED;
    float_reg_stat[i].reg_busy = 0;
    float_reg_stat[i].ROB_num = UNDEFINED;
  }
	head_ROB = 0;
	issue_success = 1;
	num_cycles = 0;
	check_end = 0;
	num_instructions = 0;
	is_cleared = 0;
	is_store_ex = 0;
  is_load_ex = 0;
  current_B = UNDEFINED;
}


// 析构函数，表明在对象销毁时并不需要额外的清理操作 
sim_ooo::~sim_ooo(){
}

// 初始化执行单元的相关参数和数据结构 
void sim_ooo::init_exec_unit(exe_unit_t exec_unit, unsigned latency, unsigned instances){
  execution_structure.exe_unit = exec_unit;
	execution_structure.latency_exe = latency;
	execution_structure.instance_exe = instances;

  create_exe_unit.push_back(execution_structure);
	switch (create_exe_unit.back().exe_unit){
    case INTEGER:
				num_latency_int.resize(create_exe_unit.back().instance_exe);
				int_ex_done.resize(create_exe_unit.back().instance_exe);
				for (unsigned j = 0; j < num_latency_int.size(); j++){
					num_latency_int[j] = create_exe_unit.back().latency_exe;
					int_ex_done[j] = 0;
				}

				for (unsigned l = 0; l < create_exe_unit.back().instance_exe; l++){
					big_int.push_back(TempReg);
				}
				break;

		case ADDER:
  			num_latency_add.resize(create_exe_unit.back().instance_exe);
				add_ex_done.resize(create_exe_unit.back().instance_exe);
				for (unsigned j = 0; j < num_latency_add.size(); j++){
  				num_latency_add[j] = create_exe_unit.back().latency_exe;
					add_ex_done[j] = 0;
  		  }

				for (unsigned l = 0; l < create_exe_unit.back().instance_exe; l++){
				  big_add.push_back(TempReg);
				}
		break;

		case MULTIPLIER:
			num_latency_mul.resize(create_exe_unit.back().instance_exe);
			mul_ex_done.resize(create_exe_unit.back().instance_exe);
			for (unsigned j = 0; j < num_latency_mul.size(); j++){
				num_latency_mul[j] = create_exe_unit.back().latency_exe;
				mul_ex_done[j] = 0;
			}

      for (unsigned l = 0; l < create_exe_unit.back().instance_exe; l++){
				big_mul.push_back(TempReg);
			}
			break;

		case DIVIDER:
			num_latency_div.resize(create_exe_unit.back().instance_exe);
			div_ex_done.resize(create_exe_unit.back().instance_exe);
			for (unsigned j = 0; j < num_latency_div.size(); j++){
				num_latency_div[j] = create_exe_unit.back().latency_exe;
				div_ex_done[j] = 0;
			}

			for (unsigned l = 0; l < create_exe_unit.back().instance_exe; l++){
				big_div.push_back(TempReg);
			}
			break;

    case MEMORY:
  		num_latency_mem.resize(create_exe_unit.back().instance_exe);
			ld_ex_done.resize(create_exe_unit.back().instance_exe);
  		for (unsigned j = 0; j < num_latency_mem.size(); j++){
  			num_latency_mem[j] = create_exe_unit.back().latency_exe;
  		}

    	for (unsigned l = 0; l < create_exe_unit.back().instance_exe; l++){
  			big_mem.push_back(TempReg);
  		}
  		break;

		default:
			break;
    }
}

// 664 从指定的文件中加载程序指令
// 从指定文件中读取程序指令，解析每条指令并存储到 instruction_memory 向量中，以便后续的模拟和执行。
void sim_ooo::load_program(const char *filename, unsigned base_address){
	//文件操作准备
  char *instr_temp;
	char temp_str[25];
	unsigned instr_scan = 0;
	unsigned word_count = 0;
	unsigned check_branch;

	string line;


	ifstream fp(filename); //文件读取
	if (!fp.is_open()){
	std::cerr << "/* error: open file */" << filename << "failed!" <<endl;
		exit(-1);
	}
     //逐行处理指令
	while(getline(fp,line)){
		word_count = 0;
		check_branch = 0;
		every_instruct.instr_address = base_address + instr_scan * 4;
        // 识别指令类型
		strcpy(temp_str,line.c_str());
		instr_temp = strtok(temp_str,"\t,: ");
		for(unsigned i = 0; i < NUM_OPCODES; i++) {
			if(strcmp(opcode_name[i],instr_temp) == 0)
			{
				check_branch = 0;
				break;
			}
			else
			{
				check_branch = 1;
			}
		}
		if(check_branch == 1)
		{
			every_instruct.branch_array = instr_temp;
		}
		else
		{
			every_instruct.branch_array = "";
			every_instruct.op_code = instr_temp;
		}
		while (instr_temp != NULL){
			instr_temp = strtok(NULL,"\t ");
			if(check_branch == 1){
				if(word_count == 0){
					every_instruct.op_code = instr_temp;
				}
				else if(word_count == 1)
				{
					if(instr_temp == NULL)
					{
						every_instruct.mand_operand = "";
					}
					else{
						every_instruct.mand_operand = instr_temp;
					}
				}
				else if(word_count == 2)
				{
					if(instr_temp == NULL){
						every_instruct.remainder_operand1 = "";
					}
					else{
						every_instruct.remainder_operand1 = instr_temp;
					}
				}
				else if(word_count == 3)
				{
					if(instr_temp == NULL){
						every_instruct.remainder_operand2 = "";
					}
					else{
						every_instruct.remainder_operand2 = instr_temp;
				}
				}
			}
			else
			{
				if(word_count == 0)
				{
					if(instr_temp == NULL){
						every_instruct.mand_operand = "";
					}
					else{
					every_instruct.mand_operand = instr_temp;
					}
				}
				else if(word_count == 1)
				{
					if(instr_temp == NULL){
					every_instruct.remainder_operand1 = "";
					}
					else{
						every_instruct.remainder_operand1 = instr_temp;
					}
				}
				else if(word_count == 2)
					{
					if(instr_temp == NULL){
						every_instruct.remainder_operand2 = "";
					}
					else{
						every_instruct.remainder_operand2 = instr_temp;
					}
				}
			}
			word_count++;
		}
		every_instruct.classify_exe = NOTVALID;
		instruction_memory.push_back(every_instruct); //存储指令信息
		instr_scan++; // 指示已处理的指令数
		// 清空临时变量,重置 
		every_instruct.mand_operand = "";
		every_instruct.remainder_operand1 = "";
		every_instruct.remainder_operand2 = "";
		every_instruct.op_code = "";
		every_instruct.branch_array = "";
	}
	
	base_add = base_address;
	current_PC = base_add;
}
//749  运行循环，按照指定的周期数或直到结束条件为止
void sim_ooo::run(unsigned cycles){
	unsigned PC_index;
	if(cycles!=0){  //如果周期数 不为零，则进入循环，执行指定次数的运行
		while (cycles!=0) {
			commit_stage();

			write_res();

			execute_ins();

			if(is_cleared != 1 ){  //如果当前状态未被清除 ，则继续发射新指令
				for(int i = 0; i < issue_size; i++){
					PC_index = (current_PC - base_add)/4;
					issue(PC_index);
					if(issue_success == 1){  
            pending_ins[current_B].log_pc = current_PC;
            pending_ins[current_B].log_issue = num_cycles;
						current_PC = current_PC + 4;
					}
					else{
						break;
					}
				}
			}
      if(if_branch == 1){  //如果发生分支预测错误，则进行状态清理以恢复到分支错误前的状态。
        clear_log();
        clear_rob();
        clear_res();
        clear_reg();
        clear_exe();
        head_ROB = 0;
        if_branch = 0;
      }

      num_cycles++;
      clear_flags();
			cycles--;
		}
	}
	else{
		while(cycles==0){ //进入无限循环，直到模拟运行完成
			if(eop_end){
				break;
			}
			else{
				commit_stage();

				write_res();

				execute_ins();

				if(is_cleared != 1){
					for(int i = 0; i < issue_size; i++){
						PC_index = (current_PC - base_add)/4;
						issue(PC_index);
						if(issue_success == 1){
              pending_ins[current_B].log_pc = current_PC;
              pending_ins[current_B].log_issue = num_cycles;
							current_PC = current_PC + 4;
						}
						else{
							break;
						}
					}
				}
        if(if_branch == 1){
          clear_log();
					clear_rob();
					clear_res();
					clear_reg();
					clear_exe();
          head_ROB = 0;
          if_branch = 0;
        }
        num_cycles++;
        clear_flags();
			}
		}
	}
}



// 清除各种标志位,0表示没有指令完成执行。
void sim_ooo::clear_flags(){
  for(unsigned i = 0; i < int_ex_done.size(); i++){
    int_ex_done[i] = 0;
  }

  for(unsigned i = 0; i < mul_ex_done.size(); i++){
    mul_ex_done[i] = 0;
  }

  for(unsigned i = 0; i < div_ex_done.size(); i++){
    div_ex_done[i] = 0;
  }

  for(unsigned i = 0; i < add_ex_done.size(); i++){
    add_ex_done[i] = 0;
  }

  for(unsigned i = 0; i < ld_ex_done.size(); i++){
    ld_ex_done[i] = 0;
  }

  for(unsigned i = 0; i < ROB_table.size(); i++){
    if(ROB_table[i].if_commit == 1){
      ROB_table[i].if_commit = 0;
    }
  }
}
 
 # 1850
void sim_ooo::issue(unsigned temp_PC){
  char operands_needed[10];
  unsigned hold_val;
  unsigned hold_wr_val = UNDEFINED;
  unsigned hold_val1;
  unsigned hold_val2;
  unsigned hold_val1_float;
  unsigned hold_val2_float;
	unsigned rob_yes;
	unsigned res_yes_int;
	unsigned res_yes_mul;
	unsigned res_yes_load;
	unsigned res_yes_add;
	unsigned imm;
	unsigned h_val;
	int fake_head;
	for(unsigned i = head_ROB; i < ROB_table.size(); i++){
		if(ROB_table[i].rob_busy != 1){  //该条目未被占用
			current_B = i;
			rob_yes = 1;
			break;
		}
		else{
			rob_yes = 0;
		}
	}

	if(rob_yes == 0){  //在第一轮遍历中未找到可用的 ROB 条目,进行第二轮遍历。0 到 head_ROB
		for(unsigned j = 0; j < head_ROB; j++){
			if(ROB_table[j].rob_busy != 1){
				current_B = j;
				rob_yes = 1;
				break;
			}
			else{
				rob_yes = 0;
			}
		}
	}
     // 检查 current_B 是否位于 ROB 表的末尾或者 head_ROB 的前一个位置。将其重置为未定义状态 (UNDEFINED)
    if(head_ROB == 0){
      if(current_B == (ROB_table.size()-1)){
        if(ROB_table[current_B].if_commit == 1){
          ROB_table[current_B].if_commit = 0;
          current_B = UNDEFINED;
          rob_yes = 0;
        }
      }
    }
    else{
      if(current_B == (head_ROB - 1)){
        if(ROB_table[current_B].if_commit == 1){
          ROB_table[current_B].if_commit = 0;
          current_B = UNDEFINED;
          rob_yes = 0;
        }
      }
    }
// 980
//清除和检查多个标志位，以确保执行单元和保留站以正确地准备和执行指令
   //866 将执行单元完成的标志位（int_ex_done、add_ex_done、mul_ex_done、ld_ex_done）重置为 0
  for(unsigned iter = 0; iter < int_ex_done.size(); iter++){
    if(int_ex_done[iter] == 1){
      int_ex_done[iter] = 0;
    }
  }

  for(unsigned iter = 0; iter < add_ex_done.size(); iter++){
    if(add_ex_done[iter] == 1){
      add_ex_done[iter] = 0;
    }
  }

  for(unsigned iter = 0; iter < mul_ex_done.size(); iter++){
    if(mul_ex_done[iter] == 1){
      mul_ex_done[iter] = 0;
    }
  }

  for(unsigned iter = 0; iter < ld_ex_done.size(); iter++){
    if(ld_ex_done[iter] == 1){
      ld_ex_done[iter] = 0;
    }
  }
    // 检查和选择可用的保留站
	for(unsigned it = 1; it < RES_Stat_Int.size(); it++){
		if(RES_Stat_Int[it].res_busy!=1){ //如果找到一个未被占用的保留站，并且其写入完成标志 (wr_done) 为 1，则将其重置为 0，并继续寻找下一个保留站。
			if(RES_Stat_Int[it].wr_done == 1){
				RES_Stat_Int[it].wr_done = 0;
				res_yes_int = 0;
				continue;
			}
			// 将 current_R_int 设置为该保留站的索引，并将 res_yes_int 设为 1。然后跳出循环。
			current_R_int = it;
			res_yes_int = 1;
			break;
		}
		else{// 没找到 
			res_yes_int = 0;
		}
	}

	for(unsigned i = 0; i < RES_Stat_Int.size(); i++){
		if(RES_Stat_Int[i].res_busy!=1){
			if(RES_Stat_Int[i].use_ex == 1){
				RES_Stat_Int[i].use_ex = 0;
				res_yes_int = 0;
				continue;
			}
			current_R_int = i;
			res_yes_int = 1;
			break;
		}
		else{
			res_yes_int = 0;
		}
	}
   // 乘法保留站 
	for(unsigned it = 1; it < RES_Stat_Mul.size(); it++){
		if(RES_Stat_Mul[it].res_busy!=1){
			if(RES_Stat_Mul[it].wr_done == 1){
				RES_Stat_Mul[it].wr_done = 0;
				res_yes_mul = 0;
				continue;
			}
			current_R_mul = it;
			res_yes_mul = 1;
		}
		else{
			res_yes_mul = 0;
		}
	}

	for(unsigned i = 0; i < RES_Stat_Mul.size(); i++){
		if(RES_Stat_Mul[i].res_busy!=1){
			if(RES_Stat_Mul[i].use_ex == 1){
				RES_Stat_Mul[i].use_ex = 0;
				res_yes_mul = 0;
				continue;
			}
			current_R_mul = i;
			res_yes_mul = 1;
			break;
		}
		else{
			res_yes_mul = 0;
		}
	}
    // 加法保留站 
	for(unsigned it = 1; it < RES_Stat_Add.size(); it++){
		if(RES_Stat_Add[it].res_busy!=1){
			if(RES_Stat_Add[it].wr_done == 1){
				RES_Stat_Add[it].wr_done = 0;
				res_yes_add = 0;
				continue;
			}
			current_R_add = it;
			res_yes_add = 1;
		}
		else{
			res_yes_add = 0;
		}
	}

	for(unsigned i = 0; i < RES_Stat_Add.size(); i++){
		if(RES_Stat_Add[i].res_busy!=1){
			if(RES_Stat_Add[i].use_ex == 1){
				RES_Stat_Add[i].use_ex = 0;
				res_yes_add = 0;
				continue;
			}
			current_R_add = i;
			res_yes_add = 1;
			break;
		}
		else{
			res_yes_add = 0;
		}
	}
    // 加载保留站 
	for(unsigned it = 1; it < RES_Stat_Load.size(); it++){
		if(RES_Stat_Load[it].res_busy!=1){
			if(RES_Stat_Load[it].wr_done == 1){
				RES_Stat_Load[it].wr_done = 0;
				res_yes_load = 0;
				continue;
			}
			current_R_ld = it;
			res_yes_load = 1;
		}
		else{
			res_yes_load = 0;
		}
	}

	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){
		if(RES_Stat_Load[i].res_busy!=1){
			if(RES_Stat_Load[i].use_ex == 1){
				RES_Stat_Load[i].use_ex = 0;
				res_yes_load = 0;
				continue;
			}
			current_R_ld = i;
			res_yes_load = 1;
			break;
		}
		else{
			res_yes_load = 0;
		}
	}
  // 1845
  //根据指令的操作码（opcode）选择执行逻辑
  switch (opcode_map[instruction_memory[temp_PC].op_code]) {
    case XOR:
  	case ADD:
  	case SUB:
  	case OR:
  	case AND:
  	// 处理 XOR、ADD、SUB、OR、AND 操作码的指令。
	//首先检查整数执行单元（res_yes_int）和ROB（rob_yes）是否都可用，如果是，则允许指令发射（issue_success = 1）。
	//清除之前完成的整数执行标志（int_ex_done）。
	//解析操作数，并根据需要设置操作数的依赖关系（Vj 和 Vk）。
	//设置当前保留站和ROB为忙碌状态，并存储指令的相关信息。
	//如果无法发射指令，则将 issue_success 设置为 0。
			if(res_yes_int == 1 && rob_yes == 1){
				issue_success = 1;
				// 清除整数执行完成标志
				for(unsigned iter = 0; iter < int_ex_done.size(); iter++){
					if(int_ex_done[iter] == 1){
						int_ex_done[iter] = 0;
					}
				}
		// 解析操作数并准备执行
      	strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
	      hold_val1 = atoi(operands_needed+1);
	      strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
	      hold_val2 = atoi(operands_needed+1);
	      // 处理第一个操作数的依赖关系
	      if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Int[current_R_int].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Int[current_R_int].Qj = 0;
						RES_Stat_Int[current_R_int].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Int[current_R_int].if_depend_Vj = 1;
						RES_Stat_Int[current_R_int].Qj = h_val+1;
					}
	      }
				else{
					RES_Stat_Int[current_R_int].Vj = register_file[hold_val1];
					RES_Stat_Int[current_R_int].Qj = 0;
					RES_Stat_Int[current_R_int].if_depend_Vj = 0;
				}
				// 标记保留站和ROB为忙碌
				RES_Stat_Int[current_R_int].res_busy = 1;
				RES_Stat_Int[current_R_int].res_dest = current_B;
				ROB_table[current_B].rob_busy = 1;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
                // 处理第二个操作数的依赖关系
				if(int_reg_stat[hold_val2].reg_busy){
					h_val = int_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Int[current_R_int].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Int[current_R_int].Qk = 0;
						RES_Stat_Int[current_R_int].if_depend_Vk = 0;
					}
					else{
						RES_Stat_Int[current_R_int].if_depend_Vk = 1;
						RES_Stat_Int[current_R_int].Qk = h_val+1;
					}
				}
				else{
					RES_Stat_Int[current_R_int].Vk = register_file[hold_val2];
					RES_Stat_Int[current_R_int].Qk = 0;
					RES_Stat_Int[current_R_int].if_depend_Vk = 0;
				}
				// 标记寄存器为忙碌状态，并更新ROB和保留站信息
				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
	            hold_val = atoi(operands_needed+1);
				int_reg_stat[hold_val].ROB_num = current_B;
				int_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = INTEGER_RES;
				RES_Stat_Int[current_R_int].res_dest_type = INTEGER_RES;
				RES_Stat_Int[current_R_int].op = INTEGER;
				RES_Stat_Int[current_R_int].op_num = temp_PC;
				RES_Stat_Int[current_R_int].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}

			break;
        // 乘法运算（MULT）
        //首先检查乘法执行单元（res_yes_mul）和ROB（rob_yes）是否都可用，如果是，则允许指令发射（issue_success = 1）。
		//清除之前完成的乘法执行标志（mul_ex_done）。
		//解析操作数，并根据需要设置操作数的依赖关系（Vj 和 Vk）。
		//设置当前乘法保留站和ROB为忙碌状态，并存储指令的相关信息。
		//如果无法发射指令，则将 issue_success 设置为 0。
		case MULT:
			if(res_yes_mul == 1 && rob_yes == 1){
				issue_success = 1;
				// 清除乘法执行完成标志
				for(unsigned iter = 0; iter < mul_ex_done.size(); iter++){
					if(mul_ex_done[iter] == 1){
						mul_ex_done[iter] = 0;
					}
				}
				// 解析操作数并准备执行
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
				hold_val1 = atoi(operands_needed+1);ROB_table[current_B].rob_busy = 1;
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
				hold_val2 = atoi(operands_needed+1);
				// 处理第一个操作数的依赖关系
				if(int_reg_stat[hold_val1].reg_busy){
					h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qj = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qj = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vj = register_file[hold_val1];
					RES_Stat_Mul[current_R_mul].Qj = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
				}
				 // 标记乘法保留站和ROB为忙碌
				RES_Stat_Mul[current_R_mul].res_busy = 1;
				RES_Stat_Mul[current_R_mul].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;
                // 处理第二个操作数的依赖关系
				if(int_reg_stat[hold_val2].reg_busy){
					h_val = int_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qk = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qk = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vk = register_file[hold_val2];
					RES_Stat_Mul[current_R_mul].Qk = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
				}
				// 标记乘法指令的寄存器为忙碌状态，并更新ROB和乘法保留站的信息
				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);
				int_reg_stat[hold_val].ROB_num = current_B;
				int_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = INTEGER_RES;
				RES_Stat_Mul[current_R_mul].res_dest_type = INTEGER_RES;
				RES_Stat_Mul[current_R_mul].op = MULTIPLIER;
				RES_Stat_Mul[current_R_mul].op_num = temp_PC;
				RES_Stat_Mul[current_R_mul].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}

			break;
        //条件判断：首先检查是否满足发射条件，即乘法执行单元可用（res_yes_mul == 1）并且ROB未忙碌（rob_yes == 1）。
		//发射指令：如果满足条件，则将 issue_success 设置为 1，表示指令成功发射。然后，清除之前完成的除法执行标志（div_ex_done）。
		//操作数准备：解析指令中的操作数，并将其存储为整数变量 hold_val1 和 hold_val2。
		//处理操作数依赖关系：对于第一个操作数 hold_val1,如果寄存器被占用（int_reg_stat[hold_val1].reg_busy），则查找其ROB编号。
		//如果对应的ROB项标记为 ready，则将其值赋给保留站 Vj，并将 Qj 设置为 0。否则，设置 if_depend_Vj 为 1，并将 Qj 设置为相应的ROB编号 + 1。
		//如果寄存器未被占用，则直接将寄存器文件中的值赋给 Vj。
		//标记保留站和ROB状态：设置当前除法保留站 RES_Stat_Mul[current_R_mul] 为忙碌状态，并分配其目的ROB编号 current_B。
		//更新ROB表项 ROB_table[current_B] 的相关信息，包括指令地址、状态等。
		//处理第二个操作数：类似地处理第二个操作数 hold_val2。
		//寄存器状态更新：解析操作数中的目标寄存器 hold_val，将当前ROB编号 current_B 分配给其ROB编号，并将寄存器标记为占用。
		//其它信息更新：更新ROB表项 ROB_table[current_B] 的目的寄存器、指令类型等信息。更新除法保留站 RES_Stat_Mul[current_R_mul] 的操作类型、指令编号、执行地址等信息。
		//发射失败处理：如果无法满足发射条件，则将 issue_success 设置为 0，表示指令未成功发射。
		case DIV:
			if(res_yes_mul == 1 && rob_yes == 1){
				issue_success = 1;
				for(unsigned iter = 0; iter < div_ex_done.size(); iter++){
					if(div_ex_done[iter] == 1){
						div_ex_done[iter] = 0;
					}
				}
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
				hold_val1 = atoi(operands_needed+1);
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
				hold_val2 = atoi(operands_needed+1);
				if(int_reg_stat[hold_val1].reg_busy){
					h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qj = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qj = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vj = register_file[hold_val1];
					RES_Stat_Mul[current_R_mul].Qj = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
				}
				RES_Stat_Mul[current_R_mul].res_busy = 1;
				RES_Stat_Mul[current_R_mul].res_dest = current_B;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				if(int_reg_stat[hold_val2].reg_busy){
					h_val = int_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qk = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qk = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vk = register_file[hold_val2];
					RES_Stat_Mul[current_R_mul].Qk = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
				}
				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);
				int_reg_stat[hold_val].ROB_num = current_B;
				int_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = INTEGER_RES;
				RES_Stat_Mul[current_R_mul].res_dest_type = INTEGER_RES;
				RES_Stat_Mul[current_R_mul].op = DIVIDER;
				RES_Stat_Mul[current_R_mul].op_num = temp_PC;
				RES_Stat_Mul[current_R_mul].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}

			break;
        // 在这段代码中，处理了整数立即数运算指令（XORI, ADDI, SUBI, ORI, ANDI）的情况
        //条件判断：首先检查是否满足发射条件，即整数执行单元可用（res_yes_int == 1）并且ROB未忙碌（rob_yes == 1）。
		//清除执行完成标志：如果满足条件，则清除之前完成的整数执行标志（int_ex_done）。
		//发射指令：将 issue_success 设置为 1，表示指令成功发射。
		//解析第一个操作数：解析指令中的第一个操作数 hold_val1。
		//处理第一个操作数的依赖关系：如果第一个操作数 hold_val1 的寄存器被占用，则查找其ROB编号，并根据ROB表中的状态设置保留站 Vj 的值。
		//如果ROB表中的对应项标记为 ready，则直接将其值赋给 Vj，并将 Qj 设置为 0。否则，设置 if_depend_Vj 为 1，并将 Qj 设置为相应的ROB编号 + 1。
		//如果寄存器未被占用，则直接将寄存器文件中的值赋给 Vj。
		//标记保留站和ROB状态：设置当前整数保留站 RES_Stat_Int[current_R_int] 为忙碌状态，并分配其目的ROB编号 current_B。更新ROB表项 ROB_table[current_B] 的相关信息，包括指令地址、状态等。
		//解析第二个操作数（立即数）：解析指令中的第二个操作数（立即数），将其转换为无符号长整型并存储在 imm 变量中。
		//更新保留站的立即数值：将解析得到的立即数值 imm 存储在保留站 RES_Stat_Int[current_R_int] 的 imm_val 中。将 Qk 设置为 0，表示第二个操作数没有依赖。
		//解析目标寄存器并更新寄存器状态：解析指令中的目标寄存器 hold_val，将当前ROB编号 current_B 分配给其ROB编号，并将寄存器标记为占用。
		//其它信息更新：更新ROB表项 ROB_table[current_B] 的目的寄存器、指令类型等信息。更新整数保留站 RES_Stat_Int[current_R_int] 的操作类型、指令编号、执行地址等信息。
		//发射失败处理：如果无法满足发射条件，则将 issue_success 设置为 0，表示指令未成功发射。
		case XORI:
		case ADDI:
		case SUBI:
		case ORI:
		case ANDI:
			char *endptr;
			if(res_yes_int == 1 && rob_yes == 1){
				for(unsigned iter = 0; iter < int_ex_done.size(); iter++){
					if(int_ex_done[iter] == 1){
						int_ex_done[iter] = 0;
					}
				}
				issue_success = 1;
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
	      hold_val1 = atoi(operands_needed+1);
	      if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Int[current_R_int].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Int[current_R_int].Qj = 0;
						RES_Stat_Int[current_R_int].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Int[current_R_int].Qj = h_val+1;
						RES_Stat_Int[current_R_int].if_depend_Vj = 1;
					}
	      }
				else{
					RES_Stat_Int[current_R_int].Vj = register_file[hold_val1];
					RES_Stat_Int[current_R_int].Qj = 0;
					RES_Stat_Int[current_R_int].if_depend_Vj = 0;
				}
				RES_Stat_Int[current_R_int].res_busy = 1;
				RES_Stat_Int[current_R_int].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;
                 // 解析第二个操作数（立即数）
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
				imm = strtoul(operands_needed, &endptr, 0);
				RES_Stat_Int[current_R_int].imm_val = imm;
				RES_Stat_Int[current_R_int].Qk = 0;
				RES_Stat_Int[current_R_int].if_depend_Vk = 0;

				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);
				int_reg_stat[hold_val].ROB_num = current_B;
				int_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = INTEGER_RES;
				RES_Stat_Int[current_R_int].op = INTEGER;
				RES_Stat_Int[current_R_int].res_dest_type = INTEGER_RES;
				RES_Stat_Int[current_R_int].op_num = temp_PC;
				RES_Stat_Int[current_R_int].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}

			break;

		case ADDS:
		case SUBS:
			if(res_yes_add == 1 && rob_yes == 1){
				issue_success = 1;
				for(unsigned iter = 0; iter < add_ex_done.size(); iter++){
					if(add_ex_done[iter] == 1){
						add_ex_done[iter] = 0;
					}
				}
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
				hold_val1 = atoi(operands_needed+1);
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
	      hold_val2 = atoi(operands_needed+1);
	      if(float_reg_stat[hold_val1].reg_busy){
	        h_val = float_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Add[current_R_add].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Add[current_R_add].Qj = 0;
						RES_Stat_Add[current_R_add].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Add[current_R_add].Qj = h_val+1;
						RES_Stat_Add[current_R_add].if_depend_Vj = 1;
					}
	      }
				else{
					RES_Stat_Add[current_R_add].Vj = float2unsigned(float(float_reg_file[hold_val1]));
					RES_Stat_Add[current_R_add].Qj = 0;
					RES_Stat_Add[current_R_add].if_depend_Vj = 0;
				}
				RES_Stat_Add[current_R_add].res_busy = 1;
				RES_Stat_Add[current_R_add].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				if(float_reg_stat[hold_val2].reg_busy){
					h_val = float_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Add[current_R_add].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Add[current_R_add].Qk = 0;
						RES_Stat_Add[current_R_add].if_depend_Vk = 0;
					}
					else{
						RES_Stat_Add[current_R_add].Qk = h_val+1;
						RES_Stat_Add[current_R_add].if_depend_Vk = 1;
					}
				}
				else{
					RES_Stat_Add[current_R_add].Vk = float2unsigned(float(float_reg_file[hold_val2]));
					RES_Stat_Add[current_R_add].Qk = 0;
					RES_Stat_Add[current_R_add].if_depend_Vk = 0;
				}

				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);
				float_reg_stat[hold_val].ROB_num = current_B;
				float_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = FLOAT_RES;
				RES_Stat_Add[current_R_add].res_dest_type = FLOAT_RES;
				RES_Stat_Add[current_R_add].op = ADDER;
				RES_Stat_Add[current_R_add].op_num = temp_PC;
				RES_Stat_Add[current_R_add].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}
			break;

		case MULTS:
			if(res_yes_mul == 1 && rob_yes == 1){
				issue_success = 1;
				for(unsigned iter = 0; iter < mul_ex_done.size(); iter++){
					if(mul_ex_done[iter] == 1){
						mul_ex_done[iter] = 0;
					}
				}
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
				hold_val1 = atoi(operands_needed+1);
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
				hold_val2 = atoi(operands_needed+1);
				if(float_reg_stat[hold_val1].reg_busy){
					h_val = float_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qj = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qj = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vj = float2unsigned(float(float_reg_file[hold_val1]));
					RES_Stat_Mul[current_R_mul].Qj = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
				}
				RES_Stat_Mul[current_R_mul].res_busy = 1;
				RES_Stat_Mul[current_R_mul].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				if(float_reg_stat[hold_val2].reg_busy){
					h_val = float_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qk = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qk = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vk = float2unsigned(float(float_reg_file[hold_val2]));
					RES_Stat_Mul[current_R_mul].Qk = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
				}
				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);
				float_reg_stat[hold_val].ROB_num = current_B;
				float_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = FLOAT_RES;
				RES_Stat_Mul[current_R_mul].op = MULTIPLIER;
				RES_Stat_Mul[current_R_mul].res_dest_type = FLOAT_RES;
				RES_Stat_Mul[current_R_mul].op_num = temp_PC;
				RES_Stat_Mul[current_R_mul].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}
			break;

		case DIVS:
			if(res_yes_mul == 1 && rob_yes == 1){
				issue_success = 1;
				for(unsigned iter = 0; iter < div_ex_done.size(); iter++){
					if(div_ex_done[iter] == 1){
						div_ex_done[iter] = 0;
					}
				}
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand1.c_str());
				hold_val1 = atoi(operands_needed+1);
				strcpy(operands_needed,instruction_memory[temp_PC].remainder_operand2.c_str());
				hold_val2 = atoi(operands_needed+1);
				if(float_reg_stat[hold_val1].reg_busy){
					h_val = float_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qj = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qj = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vj = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vj = float2unsigned(float(float_reg_file[hold_val1]));
					RES_Stat_Mul[current_R_mul].Qj = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vj = 0;
				}
				RES_Stat_Mul[current_R_mul].res_busy = 1;
				RES_Stat_Mul[current_R_mul].res_dest = current_B;
				RES_Stat_Mul[current_R_mul].res_dest_type = FLOAT_RES;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				if(float_reg_stat[hold_val2].reg_busy){
					h_val = float_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Mul[current_R_mul].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Mul[current_R_mul].Qk = 0;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
					}
					else{
						RES_Stat_Mul[current_R_mul].Qk = h_val+1;
						RES_Stat_Mul[current_R_mul].if_depend_Vk = 1;
					}
				}
				else{
					RES_Stat_Mul[current_R_mul].Vk = float2unsigned(float(float_reg_file[hold_val2]));
					RES_Stat_Mul[current_R_mul].Qk = 0;
					RES_Stat_Mul[current_R_mul].if_depend_Vk = 0;
				}
				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);
				float_reg_stat[hold_val].ROB_num = current_B;
				float_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				ROB_table[current_B].dest_type = FLOAT_RES;
				RES_Stat_Mul[current_R_mul].op = DIVIDER;
				RES_Stat_Mul[current_R_mul].op_num = temp_PC;
				RES_Stat_Mul[current_R_mul].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}
			break;

		case BEQZ:
		case BNEZ:
		case BLTZ:
		case BGTZ:
		case BLEZ:
		case BGEZ:
			if(res_yes_int == 1 && rob_yes == 1){
				for(unsigned iter = 0; iter < int_ex_done.size(); iter++){
					if(int_ex_done[iter] == 1){
						int_ex_done[iter] = 0;
					}
				}
				issue_success = 1;
				strcpy(operands_needed,instruction_memory[temp_PC].mand_operand.c_str());
				hold_val1 = atoi(operands_needed+1);
				if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Int[current_R_int].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Int[current_R_int].Qj = 0;
						RES_Stat_Int[current_R_int].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Int[current_R_int].Qj = h_val+1;
						RES_Stat_Int[current_R_int].if_depend_Vj = 1;
					}
	      }
				else{
					RES_Stat_Int[current_R_int].Vj = register_file[hold_val1];
					RES_Stat_Int[current_R_int].Qj = 0;
					RES_Stat_Int[current_R_int].if_depend_Vj = 0;
				}
				RES_Stat_Int[current_R_int].res_busy = 1;
				RES_Stat_Int[current_R_int].res_dest = current_B;
				RES_Stat_Int[current_R_int].Qk = 0;
				RES_Stat_Int[current_R_int].if_depend_Vk = 0;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				RES_Stat_Int[current_R_int].res_dest_type = FLOAT_RES;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				for(unsigned i = 0; i < instruction_memory.size(); i++){
					if(instruction_memory[temp_PC].remainder_operand1 == instruction_memory[i].branch_array){
						imm = instruction_memory[i].instr_address;
						check_branch = 1;
						RES_Stat_Int[current_R_int].target_address = imm;
					}
					else{
						check_branch = 0;
					}
				}
				RES_Stat_Int[current_R_int].op = INTEGER;
				RES_Stat_Int[current_R_int].op_num = temp_PC;
				RES_Stat_Int[current_R_int].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}

			break;

		case LW:
			unsigned hold_val_imm;
			char *pch;
			if(res_yes_load == 1 && rob_yes == 1){
				for(unsigned iter = 0; iter < ld_ex_done.size(); iter++){
					if(ld_ex_done[iter] == 1){
						ld_ex_done[iter] = 0;
					}
				}
				issue_success = 1;
				strcpy(operands_needed, instruction_memory[temp_PC].remainder_operand1.c_str());
				pch = strtok(operands_needed, " ( ");
				hold_val_imm = atoi(pch);

				RES_Stat_Load[current_R_ld].res_A = hold_val_imm;

				while(pch != NULL)
				{
					hold_val1 = atoi(pch);
					pch = strtok(NULL," R,( ");
				}

				if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Load[current_R_ld].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Load[current_R_ld].Qj = 0;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Load[current_R_ld].Qj = h_val+1;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 1;
					}
	      }
				else{
					RES_Stat_Load[current_R_ld].Vj = register_file[hold_val1];
					RES_Stat_Load[current_R_ld].Qj = 0;
					RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
				}
				RES_Stat_Load[current_R_ld].Qk = 0;
				RES_Stat_Load[current_R_ld].if_depend_Vk = 0;
				RES_Stat_Load[current_R_ld].res_busy = 1;
				RES_Stat_Load[current_R_ld].res_dest = current_B;
				ROB_table[current_B].rob_busy = 1;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;

				strcpy(operands_needed, instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);

				int_reg_stat[hold_val].ROB_num = current_B;
				int_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				RES_Stat_Load[current_R_ld].res_dest_type = INTEGER_RES;
				ROB_table[current_B].dest_type = INTEGER_RES;
				RES_Stat_Load[current_R_ld].op = MEMORY;
				RES_Stat_Load[current_R_ld].op_num = temp_PC;
				RES_Stat_Load[current_R_ld].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}
			break;

		case LWS:
			unsigned hold_val_imm1;
			char *pch1;
			if(res_yes_load == 1 && rob_yes == 1){
				issue_success = 1;
				for(unsigned iter = 0; iter < ld_ex_done.size(); iter++){
					if(ld_ex_done[iter] == 1){
						ld_ex_done[iter] = 0;
					}
				}
				strcpy(operands_needed, instruction_memory[temp_PC].remainder_operand1.c_str());
				pch1 = strtok(operands_needed, " ( ");
				hold_val_imm1 = atoi(pch1);
				RES_Stat_Load[current_R_ld].res_A = hold_val_imm1;

				while(pch1 != NULL)
				{
					hold_val1 = atoi(pch1);
					pch1 = strtok(NULL," R,( ");
				}
				if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Load[current_R_ld].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Load[current_R_ld].Qj = 0;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Load[current_R_ld].Qj = h_val+1;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 1;
					}
	      }

				else{
					RES_Stat_Load[current_R_ld].Vj = register_file[hold_val1];
					RES_Stat_Load[current_R_ld].Qj = 0;
					RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
				}
				RES_Stat_Load[current_R_ld].Qk = 0;
				RES_Stat_Load[current_R_ld].if_depend_Vk = 0;
				RES_Stat_Load[current_R_ld].res_busy = 1;
				RES_Stat_Load[current_R_ld].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				strcpy(operands_needed, instruction_memory[temp_PC].mand_operand.c_str());
				hold_val = atoi(operands_needed+1);

				float_reg_stat[hold_val].ROB_num = current_B;
				float_reg_stat[hold_val].reg_busy = 1;
				ROB_table[current_B].rob_dest = hold_val;
				RES_Stat_Load[current_R_ld].res_dest_type = FLOAT_RES;
				ROB_table[current_B].dest_type = FLOAT_RES;
				RES_Stat_Load[current_R_ld].op = MEMORY;
				RES_Stat_Load[current_R_ld].op_num = temp_PC;
				RES_Stat_Load[current_R_ld].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}
			break;

		case SW:
			unsigned hold_val_imm2;
			char *pch2;
			if(res_yes_load == 1 && rob_yes == 1){
				for(unsigned iter = 0; iter < ld_ex_done.size(); iter++){
					if(ld_ex_done[iter] == 1){
						ld_ex_done[iter] = 0;
					}
				}

				issue_success = 1;
				strcpy(operands_needed, instruction_memory[temp_PC].remainder_operand1.c_str());
				pch2 = strtok(operands_needed, " ( ");
				hold_val_imm2 = atoi(pch2);

				RES_Stat_Load[current_R_ld].res_A = hold_val_imm2;

				while(pch2 != NULL)
				{
					hold_val1 = atoi(pch2);
					pch2 = strtok(NULL," R,( ");
				}

				ROB_table[current_B].add_reg = hold_val1;

				if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Load[current_R_ld].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Load[current_R_ld].Qk = 0;
						RES_Stat_Load[current_R_ld].if_depend_Vk = 0;
						ROB_table[current_B].str_add = RES_Stat_Load[current_R_ld].Vk + RES_Stat_Load[current_R_ld].res_A;
					}
					else{
						RES_Stat_Load[current_R_ld].Vk = UNDEFINED;
						RES_Stat_Load[current_R_ld].Qk = h_val+1;
						RES_Stat_Load[current_R_ld].if_depend_Vk = 1;
						ROB_table[current_B].str_add = UNDEFINED;
					}
	      }
				else{
					RES_Stat_Load[current_R_ld].Vk = register_file[hold_val1];
					RES_Stat_Load[current_R_ld].Qk = 0;
					RES_Stat_Load[current_R_ld].if_depend_Vk = 0;
					ROB_table[current_B].str_add = RES_Stat_Load[current_R_ld].Vk + RES_Stat_Load[current_R_ld].res_A;
				}
				RES_Stat_Load[current_R_ld].res_busy = 1;
				RES_Stat_Load[current_R_ld].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				strcpy(operands_needed, instruction_memory[temp_PC].mand_operand.c_str());
				hold_val2 = atoi(operands_needed+1);
				if(int_reg_stat[hold_val2].reg_busy){
					h_val = int_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Load[current_R_ld].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Load[current_R_ld].Qj = 0;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Load[current_R_ld].Vj = UNDEFINED;
						RES_Stat_Load[current_R_ld].Qj = h_val+1;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 1;
					}
				}
				else{
					RES_Stat_Load[current_R_ld].Vj = register_file[hold_val2];
					RES_Stat_Load[current_R_ld].Qj = 0;
					RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
				}
				RES_Stat_Load[current_R_ld].res_dest_type = INTEGER_RES;
				RES_Stat_Load[current_R_ld].op = MEMORY;
				RES_Stat_Load[current_R_ld].op_num = temp_PC;
				RES_Stat_Load[current_R_ld].res_pc = temp_PC*4 + base_add;
			}
			else{
				issue_success = 0;
			}
			break;

		case SWS:
			unsigned hold_val_imm3;
			char *pch3;

			if(res_yes_load == 1 && rob_yes == 1){
				issue_success = 1;
				for(unsigned iter = 0; iter < ld_ex_done.size(); iter++){
					if(ld_ex_done[iter] == 1){
						ld_ex_done[iter] = 0;
					}
				}
				strcpy(operands_needed, instruction_memory[temp_PC].remainder_operand1.c_str());
				pch3 = strtok(operands_needed, " ( ");
				hold_val_imm3 = atoi(pch3);

				RES_Stat_Load[current_R_ld].res_A = hold_val_imm3;

				while(pch3 != NULL)
				{
					hold_val1 = atoi(pch3);
					pch3 = strtok(NULL," R,( ");
				}

				ROB_table[current_B].add_reg = hold_val1;

				if(int_reg_stat[hold_val1].reg_busy){
	        h_val = int_reg_stat[hold_val1].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Load[current_R_ld].Vk = ROB_table[h_val].rob_val;
						RES_Stat_Load[current_R_ld].Qk = 0;
						RES_Stat_Load[current_R_ld].if_depend_Vk = 0;
						ROB_table[current_B].str_add = RES_Stat_Load[current_R_ld].Vk + RES_Stat_Load[current_R_ld].res_A;
					}
					else{
						RES_Stat_Load[current_R_ld].Vk = UNDEFINED;
						RES_Stat_Load[current_R_ld].Qk = h_val+1;
						RES_Stat_Load[current_R_ld].if_depend_Vk = 1;
						ROB_table[current_B].str_add = UNDEFINED;
					}
	      }
				else{
					RES_Stat_Load[current_R_ld].Vk = register_file[hold_val1];
					RES_Stat_Load[current_R_ld].Qk = 0;
					RES_Stat_Load[current_R_ld].if_depend_Vk = 0;
					ROB_table[current_B].str_add = RES_Stat_Load[current_R_ld].Vk + RES_Stat_Load[current_R_ld].res_A;
				}
				RES_Stat_Load[current_R_ld].res_busy = 1;
				RES_Stat_Load[current_R_ld].res_dest = current_B;
				ROB_table[current_B].rob_pc = temp_PC*4 + base_add;
				ROB_table[current_B].rob_temp_pc = temp_PC;
				ROB_table[current_B].state = ISSUE;
				ROB_table[current_B].rob_busy = 1;

				strcpy(operands_needed, instruction_memory[temp_PC].mand_operand.c_str());
				hold_val2 = atoi(operands_needed+1);
				if(float_reg_stat[hold_val2].reg_busy){
					h_val = float_reg_stat[hold_val2].ROB_num;
					if(ROB_table[h_val].ready){
						RES_Stat_Load[current_R_ld].Vj = ROB_table[h_val].rob_val;
						RES_Stat_Load[current_R_ld].Qj = 0;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
					}
					else{
						RES_Stat_Load[current_R_ld].Vj = UNDEFINED;
						RES_Stat_Load[current_R_ld].Qj = h_val+1;
						RES_Stat_Load[current_R_ld].if_depend_Vj = 1;
					}
				}
				else{
					RES_Stat_Load[current_R_ld].Vj = float2unsigned(float(float_reg_file[hold_val2]));
					RES_Stat_Load[current_R_ld].Qj = 0;
					RES_Stat_Load[current_R_ld].if_depend_Vj = 0;
				}
				RES_Stat_Load[current_R_ld].res_dest_type = FLOAT_RES;
				RES_Stat_Load[current_R_ld].op = MEMORY;
				RES_Stat_Load[current_R_ld].op_num = temp_PC;
				RES_Stat_Load[current_R_ld].res_pc = temp_PC*4 + base_add;
			}

			else{
				issue_success = 0;
			}
			break;

		case EOP:
			check_end = 1;
			issue_success = 0;
			break;

		default:
			break;
  }
}

void sim_ooo::execute_ins(){
//	std::cout << "****************EXECUTE*****************" << '\n';
	unsigned hold_int;
	unsigned hold_add;
	unsigned hold_mul;
	unsigned hold_div;
	unsigned hold_mem;
	float temp_adds;
	float temp_divs;
	float temp_muls;

	for(unsigned i = 0 ; i < RES_Stat_Int.size(); i++){
		if(RES_Stat_Int[i].res_busy == 1){
			if((RES_Stat_Int[i].Qj == 0) && (RES_Stat_Int[i].Qk == 0)){
				if(RES_Stat_Int[i].if_depend_Vj == 1 || RES_Stat_Int[i].if_depend_Vk == 1){
					RES_Stat_Int[i].if_depend_Vj = 0;
					RES_Stat_Int[i].if_depend_Vk = 0;
					for(unsigned it = 0; it < int_ex_done.size(); it++){
						if(int_ex_done[it] == 1){
							int_ex_done[it] = 0;
						}
					}
					continue;
				}
				switch (RES_Stat_Int[i].op) {
					case INTEGER:
						for(unsigned sel_int_unit = 0; sel_int_unit < big_int.size(); sel_int_unit++){
							if((big_int[sel_int_unit].status == 1) && (ROB_table[RES_Stat_Int[i].res_dest].state == ISSUE)){
								if(int_ex_done[sel_int_unit] == 1){
									int_ex_done[sel_int_unit] = 0;
									continue;
								}
                pending_ins[RES_Stat_Int[i].res_dest].log_execute = num_cycles;
								ROB_table[RES_Stat_Int[i].res_dest].state = EXECUTE;
								RES_Stat_Int[i].use_ex = 1;
                RES_Stat_Int[i].execution_unit_used = sel_int_unit;
								big_int[sel_int_unit].status = 0;
								big_int[sel_int_unit].res_stat_num = i;
								//num_latency_int[sel_int_unit]--;
								break;
							}
						}
						break;

					default:
						break;
				}
			}
		}
	}

	for(unsigned sel_int_unit = 0; sel_int_unit < big_int.size(); sel_int_unit++){
		if(big_int[sel_int_unit].status == 0){
			if(num_latency_int[sel_int_unit] > 1){
				num_latency_int[sel_int_unit]--;
				flag_int[big_int[sel_int_unit].res_stat_num] = 0;
			}
			else{
				flag_int[big_int[sel_int_unit].res_stat_num] = 1;
				switch(opcode_map[instruction_memory[RES_Stat_Int[big_int[sel_int_unit].res_stat_num].op_num].op_code]){
				case AND:
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj & RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vk;
					break;

				case ADD:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj + RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vk;
					break;

				case OR:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj | RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vk;
					break;

				case XOR:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj ^ RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vk;
					break;

				case SUB:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj - RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vk;
					break;

				case ADDI:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj + RES_Stat_Int[big_int[sel_int_unit].res_stat_num].imm_val;
					break;

				case SUBI:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj - RES_Stat_Int[big_int[sel_int_unit].res_stat_num].imm_val;
					break;

				case XORI:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj ^ RES_Stat_Int[big_int[sel_int_unit].res_stat_num].imm_val;
					break;

				case ORI:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj | RES_Stat_Int[big_int[sel_int_unit].res_stat_num].imm_val;
					break;

				case ANDI:
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj & RES_Stat_Int[big_int[sel_int_unit].res_stat_num].imm_val;
					break;

				case BEQZ:
					if(RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj == 0){
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 1;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					}
					else{
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 0;
						branch_cond = 0;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].res_pc + 4;
					}
					break;

				case BNEZ:
					if(RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj != 0){
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 1;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					}
					else{
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 0;
						branch_cond = 0;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].res_pc + 4;
					}
					break;

				case BLTZ:
					if(RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj < 0){
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 1;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					}
					else{
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 0;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].res_pc + 4;
					}
					break;

				case BGTZ:
					if(RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj > 0){
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 1;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					}
					else{
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 0;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].res_pc + 4;
					}
					break;

				case BLEZ:
					if(RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj <= 0){
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					}
					else{
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 0;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].res_pc + 4;
					}
					break;

				case BGEZ:
					if(RES_Stat_Int[big_int[sel_int_unit].res_stat_num].Vj >= 0){
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 1;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					}
					else{
						RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 0;
						int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].res_pc + 4;
					}
					break;

				case JUMP:
					RES_Stat_Int[big_int[sel_int_unit].res_stat_num].if_branch = 1;
					int_result[big_int[sel_int_unit].res_stat_num] = RES_Stat_Int[big_int[sel_int_unit].res_stat_num].target_address;
					break;

				default:
					break;
				}

				for(unsigned a = 0; a < create_exe_unit.size(); a++){
					if(create_exe_unit[a].exe_unit == INTEGER){
						hold_int = a;
					}
				}

				num_latency_int[sel_int_unit] = create_exe_unit[hold_int].latency_exe;
				big_int[sel_int_unit].status = 1;
				int_ex_done[sel_int_unit] = 1;
			}
			
		}
		else{
			int_struc_hazard = 1;
		}
	}
	for(unsigned i = 0; i < RES_Stat_Add.size(); i++){
	
		if(RES_Stat_Add[i].res_busy == 1){
			if((RES_Stat_Add[i].Qj == 0) && (RES_Stat_Add[i].Qk == 0)){
				if(RES_Stat_Add[i].if_depend_Vj == 1 || RES_Stat_Add[i].if_depend_Vk == 1){
					RES_Stat_Add[i].if_depend_Vj = 0;
					RES_Stat_Add[i].if_depend_Vk = 0;
					for(unsigned it = 0; it < add_ex_done.size(); it++){
						if(add_ex_done[it] == 1){
							add_ex_done[it] = 0;
						}
					}
		
					continue;
				}
				switch (RES_Stat_Add[i].op){
					case ADDER:
						for(unsigned sel_add_unit = 0; sel_add_unit < big_add.size(); sel_add_unit++){
		
							if((big_add[sel_add_unit].status == 1) && (ROB_table[RES_Stat_Add[i].res_dest].state == ISSUE)){
								if(add_ex_done[sel_add_unit] == 1){
		
									add_ex_done[sel_add_unit] = 0;
									continue;
								}

				
                RES_Stat_Add[i].execution_unit_used = sel_add_unit;
                pending_ins[RES_Stat_Add[i].res_dest].log_execute = num_cycles;
								ROB_table[RES_Stat_Add[i].res_dest].state = EXECUTE;
								RES_Stat_Add[i].use_ex = 1;
								big_add[sel_add_unit].status = 0;
								big_add[sel_add_unit].res_stat_num = i;
								break;
							}
						}
						break;

					default:
						break;
				}
			}
		}
	}

	for(unsigned sel_add_unit = 0; sel_add_unit < big_add.size(); sel_add_unit++){

		if(big_add[sel_add_unit].status == 0){
			if(num_latency_add[sel_add_unit] > 1){
	
				num_latency_add[sel_add_unit]--;
				flag_add[big_add[sel_add_unit].res_stat_num] = 0;
			}
			else{
	
				flag_add[big_add[sel_add_unit].res_stat_num] = 1;
				switch (opcode_map[instruction_memory[RES_Stat_Add[big_add[sel_add_unit].res_stat_num].op_num].op_code]) {
					case ADDS:
							temp_adds = unsigned2float(RES_Stat_Add[big_add[sel_add_unit].res_stat_num].Vj) + unsigned2float(RES_Stat_Add[big_add[sel_add_unit].res_stat_num].Vk);
							add_result[big_add[sel_add_unit].res_stat_num] = float2unsigned(float(temp_adds));
							break;

						case SUBS:
							temp_adds = unsigned2float(RES_Stat_Add[big_add[sel_add_unit].res_stat_num].Vj) - unsigned2float(RES_Stat_Add[big_add[sel_add_unit].res_stat_num].Vk);
							add_result[big_add[sel_add_unit].res_stat_num] = float2unsigned(float(temp_adds));
							break;
					}
					for(unsigned a = 0; a < create_exe_unit.size(); a++){
						if(create_exe_unit[a].exe_unit == ADDER){
							hold_add = a;
						}
					}
					num_latency_add[sel_add_unit] = create_exe_unit[hold_add].latency_exe;
					big_add[sel_add_unit].status = 1;
					add_ex_done[sel_add_unit] = 1;
				}
			}
	}

	for(unsigned i = 0; i < RES_Stat_Mul.size(); i++){

		if(RES_Stat_Mul[i].res_busy == 1){
	
			if((RES_Stat_Mul[i].Qj == 0) && (RES_Stat_Mul[i].Qk == 0)){
				if(RES_Stat_Mul[i].if_depend_Vj == 1 || RES_Stat_Mul[i].if_depend_Vk == 1){
					RES_Stat_Mul[i].if_depend_Vj = 0;
					RES_Stat_Mul[i].if_depend_Vk = 0;
					for(unsigned it = 0; it < mul_ex_done.size(); it++){
						if(mul_ex_done[it] == 1){
							mul_ex_done[it] = 0;
						}
					}
					continue;
				}
	
				switch (RES_Stat_Mul[i].op) {
					case MULTIPLIER:
						for(unsigned sel_mul_unit = 0; sel_mul_unit < big_mul.size(); sel_mul_unit++){
							if((big_mul[sel_mul_unit].status == 1) && (ROB_table[RES_Stat_Mul[i].res_dest].state == ISSUE)){
								if(mul_ex_done[sel_mul_unit] == 1){
									mul_ex_done[sel_mul_unit] = 0;
									continue;
								}

								big_mul[sel_mul_unit].status = 0;
                pending_ins[RES_Stat_Mul[i].res_dest].log_execute = num_cycles;
								ROB_table[RES_Stat_Mul[i].res_dest].state = EXECUTE;
								RES_Stat_Mul[i].use_ex = 1;
								big_mul[sel_mul_unit].res_stat_num = i;
                RES_Stat_Mul[i].execution_unit_used = sel_mul_unit;
								break;
							}
						}
						break;

					case DIVIDER:
						for(unsigned sel_div_unit = 0; sel_div_unit < big_div.size(); sel_div_unit++){
	
							if(big_div[sel_div_unit].status == 1 && ROB_table[RES_Stat_Mul[i].res_dest].state == ISSUE){
		
								if(div_ex_done[sel_div_unit] == 1){
									div_ex_done[sel_div_unit] = 0;
									continue;
								}

		
								big_div[sel_div_unit].status = 0;
                pending_ins[RES_Stat_Mul[i].res_dest].log_execute = num_cycles;
								ROB_table[RES_Stat_Mul[i].res_dest].state = EXECUTE;
								big_div[sel_div_unit].res_stat_num = i;
                RES_Stat_Mul[i].execution_unit_used = sel_div_unit;
                RES_Stat_Mul[i].use_ex = 1;
               break;
							}
             
						}
						break;

					default:
						break;
							}
						}
				}
			}


	for(unsigned sel_mul_unit = 0; sel_mul_unit < big_mul.size(); sel_mul_unit++){
		if(big_mul[sel_mul_unit].status == 0){
			if(num_latency_mul[sel_mul_unit] > 1){
				num_latency_mul[sel_mul_unit]--;
				flag_mul[big_mul[sel_mul_unit].res_stat_num] = 0;
			}
			else{
		
				flag_mul[big_mul[sel_mul_unit].res_stat_num] = 1;
				switch (opcode_map[instruction_memory[RES_Stat_Mul[big_mul[sel_mul_unit].res_stat_num].op_num].op_code]) {
					case MULT:
						mul_result[big_mul[sel_mul_unit].res_stat_num] = RES_Stat_Mul[big_mul[sel_mul_unit].res_stat_num].Vj * RES_Stat_Mul[big_mul[sel_mul_unit].res_stat_num].Vk;
						break;

			  	case MULTS:
						temp_muls = unsigned2float(RES_Stat_Mul[big_mul[sel_mul_unit].res_stat_num].Vj) * unsigned2float(RES_Stat_Mul[big_mul[sel_mul_unit].res_stat_num].Vk);
						mul_result[big_mul[sel_mul_unit].res_stat_num] = float2unsigned(float(temp_muls));
						break;
					}
					for(unsigned a = 0; a < create_exe_unit.size(); a++){
						if(create_exe_unit[a].exe_unit == MULTIPLIER){
							hold_mul = a;
						}
					}
					num_latency_mul[sel_mul_unit] = create_exe_unit[hold_mul].latency_exe;
				  big_mul[sel_mul_unit].status = 1;
					mul_ex_done[sel_mul_unit] = 1;
			}
		}
	}
	for(unsigned sel_div_unit = 0; sel_div_unit < big_div.size(); sel_div_unit++){
		if(big_div[sel_div_unit].status == 0){
			if(num_latency_div[sel_div_unit] > 1){
				num_latency_div[sel_div_unit]--;
				flag_mul[big_div[sel_div_unit].res_stat_num] = 0;
			}
			else{
				flag_mul[big_div[sel_div_unit].res_stat_num] = 1;
				switch (opcode_map[instruction_memory[RES_Stat_Mul[big_div[sel_div_unit].res_stat_num].op_num].op_code]) {
					case DIV:
						mul_result[big_div[sel_div_unit].res_stat_num] = RES_Stat_Mul[big_div[sel_div_unit].res_stat_num].Vj / RES_Stat_Mul[big_div[sel_div_unit].res_stat_num].Vk;
						break;

					case DIVS:
						temp_divs = unsigned2float(RES_Stat_Mul[big_div[sel_div_unit].res_stat_num].Vj) / unsigned2float(RES_Stat_Mul[big_div[sel_div_unit].res_stat_num].Vk);
						mul_result[big_div[sel_div_unit].res_stat_num] = float2unsigned(float(temp_divs));
						break;
				}
				for(unsigned a = 0; a < create_exe_unit.size(); a++){
					if(create_exe_unit[a].exe_unit == DIVIDER){
						hold_div = a;
					}
				}
				num_latency_div[sel_div_unit] = create_exe_unit[hold_div].latency_exe;
				big_div[sel_div_unit].status = 1;
				div_ex_done[sel_div_unit] = 1;
			}
		}
	}
	unsigned store_check = 0;
	unsigned sw_check_head;

	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){
		if(RES_Stat_Load[i].res_busy == 1){
			if(ld_step1[i] == 0){
				switch (opcode_map[instruction_memory[RES_Stat_Load[i].op_num].op_code]) {
					case LW:
					case LWS:
		
						if((ROB_table[RES_Stat_Load[i].res_dest].entry - 1) >= (head_ROB)){
							for(int a = (ROB_table[RES_Stat_Load[i].res_dest].entry - 1); a >= head_ROB ; a--){
		
								if((opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SW)	||(opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SWS)){
									
									if(int_reg_stat[ROB_table[a].add_reg].reg_busy != 1){
										store_check = 1;
		
									}
									else{
		
										store_check = 0;
                    break;
									}
                 
								}
								else{
		
									store_check = 1;
								}
							}
						}
						else{
							for(int a = (ROB_table[RES_Stat_Load[i].res_dest].entry - 1); a >= 0; a--){
			
								if((opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SW)
								||(opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SWS)){
								
									if(int_reg_stat[ROB_table[a].add_reg].reg_busy != 1){
			
										store_check = 1;
									
									}
									else{
			
										store_check = 0;
                    break;
									}
								
								}
								else{
			
									store_check = 1;
								}
							}
							
								for(int a = (ROB_table.size() - 1) ; a >= head_ROB; a--){
									if((opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SW)
									||(opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SWS)){
		
										if(int_reg_stat[ROB_table[a].add_reg].reg_busy != 1){
      
                      store_check = 1;
											break;
										}
										else{
    
											store_check = 0;
										}
									}
									else{
		
										store_check = 1;
									}
								}
						
					}

					if(RES_Stat_Load[i].Qj == 0 && store_check == 1){
		
						temp_res_add[i] = RES_Stat_Load[i].Vj + RES_Stat_Load[i].res_A;
		
						ld_step1[i] = 1;
						ld_current_exe[i] = 1;
					}
					break;
				}
			}
		}
	}

	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){
		if(ld_step1[i] == 1 && ld_step2[i] == 0){

			if((ROB_table[RES_Stat_Load[i].res_dest].entry - 1) >= (head_ROB)){
				for(int a = (ROB_table[RES_Stat_Load[i].res_dest].entry - 1); a >= head_ROB ; a--){
					if((opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SW)
					||(opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SWS)){
	
						if(temp_res_add[i] == ROB_table[a].str_add){

							needs_ex_ld[i] = 0;
              if(ROB_table[a].is_available){
                RES_Stat_Load[i].from_forward = ROB_table[a].rob_val;
                RES_Stat_Load[i].address_raw = 0;
                if(ld_current_exe[i] == 1){
        					ld_current_exe[i] = 0;
        					continue;
        				}
              }
              else{
							  RES_Stat_Load[i].address_raw = a+1;
              }
  
							break;
						}
						else{
 
							needs_ex_ld[i] = 1;
							RES_Stat_Load[i].address_raw = 0;
						}
					}
					else{
	
						needs_ex_ld[i] = 1;
						RES_Stat_Load[i].address_raw = 0;
					}
				}
			}
			else{
				for(int a = (ROB_table[RES_Stat_Load[i].res_dest].entry - 1); a >= 0; a--){
					if((opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SW)
					||(opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SWS)){
	
						if(temp_res_add[i] == ROB_table[a].str_add){
							needs_ex_ld[i] = 0;
              if(ROB_table[a].is_available){
                RES_Stat_Load[i].from_forward = ROB_table[a].rob_val;
                RES_Stat_Load[i].address_raw = 0;
                if(ld_current_exe[i] == 1){
        					ld_current_exe[i] = 0;
        					continue;
        				}
              }
              else{
							  RES_Stat_Load[i].address_raw = a+1;
              }

							break;
						}
						else{
							RES_Stat_Load[i].address_raw = 0;
							needs_ex_ld[i] = 1;

						}
					}
					else{
						needs_ex_ld[i] = 1;
						RES_Stat_Load[i].address_raw = 0;

					}
				}
			
					for(int a = (ROB_table.size() - 1) ; a >= head_ROB; a--){
						if((opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SW)
						||(opcode_map[instruction_memory[ROB_table[a].rob_temp_pc].op_code] == SWS)){
							if(temp_res_add[i] == ROB_table[a].str_add){
								needs_ex_ld[i] = 0;

                if(ROB_table[a].is_available){
                  RES_Stat_Load[i].from_forward = ROB_table[a].rob_val;
                  RES_Stat_Load[i].address_raw = 0;
                  if(ld_current_exe[i] == 1){
          					ld_current_exe[i] = 0;
          				}
                }
                else{
  							  RES_Stat_Load[i].address_raw = a+1;
                }
								break;
							}
							else{
								RES_Stat_Load[i].address_raw = 0;
								needs_ex_ld[i] = 1;
							}
						}
					}
				
			}
      ld_step2[i] = 1;
		}
	}

	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){
		if(needs_ex_ld[i] == 1){
			if(RES_Stat_Load[i].if_depend_Vj == 1 || RES_Stat_Load[i].if_depend_Vk == 1){
				RES_Stat_Load[i].if_depend_Vj = 0;
				RES_Stat_Load[i].if_depend_Vk = 0;
				for(unsigned it = 0; it < ld_ex_done.size(); it++){
					if(ld_ex_done[it] == 1){
						ld_ex_done[it] = 0;
					}
				}
				continue;
			}
			switch (opcode_map[instruction_memory[RES_Stat_Load[i].op_num].op_code]) {
				case LW:
				case LWS:
						for(unsigned sel_ld_unit = 0; sel_ld_unit < big_mem.size(); sel_ld_unit++){
							if(big_mem[sel_ld_unit].status == 1 && ROB_table[RES_Stat_Load[i].res_dest].state == ISSUE){
								if(ld_ex_done[sel_ld_unit] == 1){
									ld_ex_done[sel_ld_unit] = 0;
									continue;
								}

								big_mem[sel_ld_unit].status = 0;
                pending_ins[RES_Stat_Load[i].res_dest].log_execute = num_cycles;
								ROB_table[RES_Stat_Load[i].res_dest].state = EXECUTE;
								RES_Stat_Load[i].use_ex = 1;
                RES_Stat_Load[i].execution_unit_used = sel_ld_unit;
								big_mem[sel_ld_unit].res_stat_num = i;
								RES_Stat_Load[i].res_A = temp_res_add[i];
                is_load_ex = 1;
								break;
							}
						}
					break;

				default:
					break;
			}
		}
		else if(needs_ex_ld[i] == 0){
	
			if(RES_Stat_Load[i].address_raw == 0){
	
				if(ld_current_exe[i] == 1){
					ld_current_exe[i] = 0;
					continue;
				}
				RES_Stat_Load[i].Vk = RES_Stat_Load[i].from_forward;
				mem_result[i] = RES_Stat_Load[i].from_forward;
				ld_current_exe[i] = 0;
				ld_step1[i] = 0;
        pending_ins[RES_Stat_Load[i].res_dest].log_execute = num_cycles;
				ROB_table[RES_Stat_Load[i].res_dest].state = EXECUTE;
				flag_mem[i] = 1;
				RES_Stat_Load[i].res_A = temp_res_add[i];
				needs_ex_ld[i] = UNDEFINED;
        ld_step2[i] = 0;
			}
		}
	}

	for(unsigned sel_ld_unit = 0; sel_ld_unit < big_mem.size(); sel_ld_unit++){
 
		if(is_store_ex == 0){

			if(big_mem[sel_ld_unit].status == 0){

				if(num_latency_mem[sel_ld_unit] > 1){
	
					num_latency_mem[sel_ld_unit]--;
					flag_mem[big_mem[sel_ld_unit].res_stat_num] = 0;
				}
				else{
					switch (opcode_map[instruction_memory[RES_Stat_Load[big_mem[sel_ld_unit].res_stat_num].op_num].op_code]) {
						case LW:
						case LWS:
							flag_mem[big_mem[sel_ld_unit].res_stat_num] = 1;
							mem_result[big_mem[sel_ld_unit].res_stat_num] = char2unsigned(data_memory + RES_Stat_Load[big_mem[sel_ld_unit].res_stat_num].res_A);
							break;
						}
						for(unsigned a = 0; a < create_exe_unit.size(); a++){
							if(create_exe_unit[a].exe_unit == MEMORY){
								hold_mem = a;
							}
						}
						ld_ex_done[sel_ld_unit] = 1;
						num_latency_mem[sel_ld_unit] = create_exe_unit[hold_mem].latency_exe;
						big_mem[sel_ld_unit].status = 1;
						ld_current_exe[big_mem[sel_ld_unit].res_stat_num] = 0;
						ld_step1[big_mem[sel_ld_unit].res_stat_num] = 0;
						needs_ex_ld[big_mem[sel_ld_unit].res_stat_num] = UNDEFINED;
           
            ld_step2[big_mem[sel_ld_unit].res_stat_num] = 0;
				}
			}
		}
	}



	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){

		if(RES_Stat_Load[i].res_busy){
			switch (opcode_map[instruction_memory[RES_Stat_Load[i].op_num].op_code]) {
				case SW:
						if((RES_Stat_Load[i].Qj == 0) && (RES_Stat_Load[i].Qk == 0)){
							if(RES_Stat_Load[i].if_depend_Vj == 1 || RES_Stat_Load[i].if_depend_Vk == 1){
								RES_Stat_Load[i].if_depend_Vj = 0;
								RES_Stat_Load[i].if_depend_Vk = 0;
							
								continue;
							}
						
							ROB_table[RES_Stat_Load[i].res_dest].rob_dest = RES_Stat_Load[i].Vk + RES_Stat_Load[i].res_A;
							RES_Stat_Load[i].res_A = RES_Stat_Load[i].Vk + RES_Stat_Load[i].res_A;
							ROB_table[RES_Stat_Load[i].res_dest].dest_type = STORE_DEST;
              pending_ins[RES_Stat_Load[i].res_dest].log_execute = num_cycles;
							ROB_table[RES_Stat_Load[i].res_dest].state = EXECUTE;
							flag_mem[i] = 1;
              ROB_table[RES_Stat_Load[i].res_dest].is_available = 1;
						}
					break;

				case SWS:
		
					if((RES_Stat_Load[i].Qj == 0) && (RES_Stat_Load[i].Qk == 0)){
		
						if(RES_Stat_Load[i].if_depend_Vj == 1 || RES_Stat_Load[i].if_depend_Vk == 1){
		
							RES_Stat_Load[i].if_depend_Vj = 0;
							RES_Stat_Load[i].if_depend_Vk = 0;
								
							continue;
						}
						
						ROB_table[RES_Stat_Load[i].res_dest].rob_dest = RES_Stat_Load[i].Vk + RES_Stat_Load[i].res_A;
    
						RES_Stat_Load[i].res_A = RES_Stat_Load[i].Vk + RES_Stat_Load[i].res_A;
						ROB_table[RES_Stat_Load[i].res_dest].dest_type = STORE_DEST;
            pending_ins[RES_Stat_Load[i].res_dest].log_execute = num_cycles;
						ROB_table[RES_Stat_Load[i].res_dest].state = EXECUTE;
						flag_mem[i] = 1;
            ROB_table[RES_Stat_Load[i].res_dest].is_available = 1;
					}
			break;
			}
		}
	}

}

void sim_ooo::write_res(){
	unsigned b_rob;

	for(unsigned i = 0; i < RES_Stat_Int.size(); i++){
		if(flag_int[i] == 1){
			b_rob = RES_Stat_Int[i].res_dest + 1;
			RES_Stat_Int[i].res_busy = 0;

			for(unsigned j = 0; j < RES_Stat_Int.size(); j++){
				if(RES_Stat_Int[j].Qj == b_rob){
					RES_Stat_Int[j].Vj = int_result[i];
					RES_Stat_Int[j].Qj = 0;
				}
				if(RES_Stat_Int[j].Qk == b_rob){
					RES_Stat_Int[j].Vk = int_result[i];
					RES_Stat_Int[j].Qk = 0;
				}
			}

			for(unsigned j = 0; j < RES_Stat_Add.size(); j++){
				if(RES_Stat_Add[j].Qj == b_rob){
					RES_Stat_Add[j].Vj = int_result[i];
					RES_Stat_Add[j].Qj = 0;
				}
				if(RES_Stat_Add[j].Qk == b_rob){
					RES_Stat_Add[j].Vk = int_result[i];
					RES_Stat_Add[j].Qk = 0;
				}
			}

			for(unsigned j=0; j < RES_Stat_Mul.size(); j++){
				if(RES_Stat_Mul[j].Qj == b_rob){
					RES_Stat_Mul[j].Vj = int_result[i];
					RES_Stat_Mul[j].Qj = 0;
				}
				if(RES_Stat_Mul[j].Qk == b_rob){
					RES_Stat_Mul[j].Vk = int_result[i];
					RES_Stat_Mul[j].Qk = 0;
				}
			}

			for(unsigned j = 0; j < RES_Stat_Load.size(); j++){
				if(RES_Stat_Load[j].Qj == b_rob){
					RES_Stat_Load[j].Vj = int_result[i];
					RES_Stat_Load[j].Qj = 0;
				}
				if(RES_Stat_Load[j].Qk == b_rob){
					RES_Stat_Load[j].Vk = int_result[i];
          ROB_table[RES_Stat_Load[j].res_dest].is_available = 1;
          ROB_table[RES_Stat_Load[j].res_dest].str_add = RES_Stat_Load[j].Vk + RES_Stat_Load[j].res_A;
					RES_Stat_Load[j].Qk = 0;
				}
			}


				ROB_table[RES_Stat_Int[i].res_dest].rob_val = int_result[i];
        int_ex_done[RES_Stat_Int[i].execution_unit_used] = 1;
				ROB_table[RES_Stat_Int[i].res_dest].if_branch_rob = RES_Stat_Int[i].if_branch;
				int_result[i] = UNDEFINED;
				ROB_table[b_rob - 1].ready = 1;
        pending_ins[RES_Stat_Int[i].res_dest].log_write = num_cycles;
				ROB_table[b_rob - 1].state = WRITE_RESULT;
				RES_Stat_Int[i].res_pc = UNDEFINED;
				RES_Stat_Int[i].Vj = UNDEFINED;
				RES_Stat_Int[i].Vk = UNDEFINED;
				RES_Stat_Int[i].Qj = UNDEFINED;
				RES_Stat_Int[i].res_dest = UNDEFINED;
				RES_Stat_Int[i].res_A = UNDEFINED;
				RES_Stat_Int[i].op_num = UNDEFINED;
				RES_Stat_Int[i].if_branch = UNDEFINED;
				RES_Stat_Int[i].use_ex = 1;
				RES_Stat_Int[i].op = NOTVALID;
				RES_Stat_Int[i].wr_done = 1;
				flag_int[i] = 0;
			}
	}

	for(unsigned i = 0; i < RES_Stat_Add.size(); i++){
		if(flag_add[i] == 1){
			b_rob = RES_Stat_Add[i].res_dest + 1;
			RES_Stat_Add[i].res_busy = 0;

			for(unsigned j = 0; j < RES_Stat_Int.size(); j++){
				if(RES_Stat_Int[j].Qj == b_rob){
					RES_Stat_Int[j].Vj = add_result[i];
					RES_Stat_Int[j].Qj = 0;
				}
				if(RES_Stat_Int[j].Qk == b_rob){
					RES_Stat_Int[j].Vk = add_result[i];
					RES_Stat_Int[j].Qk = 0;
				}
			}

			for(unsigned j = 0; j < RES_Stat_Add.size(); j++){
				if(RES_Stat_Add[j].Qj == b_rob){
					RES_Stat_Add[j].Vj = add_result[i];
					RES_Stat_Add[j].Qj = 0;
				}
			  if(RES_Stat_Add[j].Qk == b_rob){
					RES_Stat_Add[j].Vk = add_result[i];
					RES_Stat_Add[j].Qk = 0;
				}
			}

			for(unsigned j=0; j < RES_Stat_Mul.size(); j++){
				if(RES_Stat_Mul[j].Qj == b_rob){
					RES_Stat_Mul[j].Vj = add_result[i];
					RES_Stat_Mul[j].Qj = 0;
				}
				if(RES_Stat_Mul[j].Qk == b_rob){
					RES_Stat_Mul[j].Vk = add_result[i];
          RES_Stat_Mul[j].Qk = 0;
				}
			}

			for(unsigned j = 0; j < RES_Stat_Load.size(); j++){
				if(RES_Stat_Load[j].Qj == b_rob){
	
					RES_Stat_Load[j].Vj = add_result[i];
					RES_Stat_Load[j].Qj = 0;
				}
				if(RES_Stat_Load[j].Qk == b_rob){
		
					RES_Stat_Load[j].Vk = add_result[i];
          ROB_table[RES_Stat_Load[j].res_dest].str_add = RES_Stat_Load[j].Vk + RES_Stat_Load[j].res_A;
          ROB_table[RES_Stat_Load[j].res_dest].is_available = 1;
          RES_Stat_Load[j].Qk = 0;
				}
			}
	
				ROB_table[RES_Stat_Add[i].res_dest].rob_val = add_result[i];
				add_result[i] = UNDEFINED;
				add_ex_done[RES_Stat_Add[i].execution_unit_used] = 1;
				ROB_table[b_rob-1].ready = 1;
        pending_ins[RES_Stat_Add[i].res_dest].log_write = num_cycles;
				ROB_table[b_rob-1].state = WRITE_RESULT;
				RES_Stat_Add[i].res_busy = 0;
				RES_Stat_Add[i].res_pc = UNDEFINED;
				RES_Stat_Add[i].Vj = UNDEFINED;
				RES_Stat_Add[i].Vk = UNDEFINED;
				RES_Stat_Add[i].Qj = UNDEFINED;
				RES_Stat_Add[i].res_dest = UNDEFINED;
				RES_Stat_Add[i].res_A = UNDEFINED;
				RES_Stat_Add[i].op_num = UNDEFINED;
				RES_Stat_Add[i].if_branch = UNDEFINED;
				RES_Stat_Add[i].use_ex = 1;
				RES_Stat_Add[i].op = NOTVALID;
				RES_Stat_Add[i].wr_done = 1;
				flag_add[i] = 0;
			}
	}

	for(unsigned i = 0; i < RES_Stat_Mul.size(); i++){
	
		if(flag_mul[i] == 1){
	
			b_rob = RES_Stat_Mul[i].res_dest + 1;
			RES_Stat_Mul[i].res_busy = 0;

			for(unsigned j = 0; j < RES_Stat_Int.size(); j++){
				if(RES_Stat_Int[j].Qj == b_rob){
					RES_Stat_Int[j].Vj = mul_result[i];
					RES_Stat_Int[j].Qj = 0;
				}
				if(RES_Stat_Int[j].Qk == b_rob){
					RES_Stat_Int[j].Vk = mul_result[i];
					RES_Stat_Int[j].Qk = 0;
				}
			}


			for(unsigned j = 0; j < RES_Stat_Add.size(); j++){
				if(RES_Stat_Add[j].Qj == b_rob){
					RES_Stat_Add[j].Vj = mul_result[i];
					RES_Stat_Add[j].Qj = 0;
				}
				if(RES_Stat_Add[j].Qk == b_rob){
					RES_Stat_Add[j].Vk = mul_result[i];
					RES_Stat_Add[j].Qk = 0;
				}
			}


			for(unsigned j=0; j < RES_Stat_Mul.size(); j++){
				if(RES_Stat_Mul[j].Qj == b_rob){
					RES_Stat_Mul[j].Vj = mul_result[i];
					RES_Stat_Mul[j].Qj = 0;
				}
				if(RES_Stat_Mul[j].Qk == b_rob){
					RES_Stat_Mul[j].Vk = mul_result[i];
					RES_Stat_Mul[j].Qk = 0;
				}
			}

			for(unsigned j = 0; j < RES_Stat_Load.size(); j++){
				if(RES_Stat_Load[j].Qj == b_rob){
					RES_Stat_Load[j].Vj = mul_result[i];
					RES_Stat_Load[j].Qj = 0;
				}
				if(RES_Stat_Load[j].Qk == b_rob){
					RES_Stat_Load[j].Vk = mul_result[i];
          ROB_table[RES_Stat_Load[j].res_dest].str_add = RES_Stat_Load[j].Vk + RES_Stat_Load[j].res_A;
          ROB_table[RES_Stat_Load[j].res_dest].is_available = 1;
          RES_Stat_Load[j].Qk = 0;
				}
			}
        switch (opcode_map[instruction_memory[RES_Stat_Mul[i].op_num].op_code]) {
          case MULT:
          case MULTS:
            mul_ex_done[RES_Stat_Mul[i].execution_unit_used] = 1;
            break;

          case DIV:
          case DIVS:
            div_ex_done[RES_Stat_Mul[i].execution_unit_used] = 1;
            break;
        }
				ROB_table[RES_Stat_Mul[i].res_dest].rob_val = mul_result[i];
				mul_result[i] = UNDEFINED;
				ROB_table[b_rob-1].ready = 1;
        pending_ins[RES_Stat_Mul[i].res_dest].log_write = num_cycles;
				ROB_table[b_rob-1].state = WRITE_RESULT;
				RES_Stat_Mul[i].res_busy = 0;
				RES_Stat_Mul[i].res_pc = UNDEFINED;
				RES_Stat_Mul[i].Vj = UNDEFINED;
				RES_Stat_Mul[i].Vk = UNDEFINED;
				RES_Stat_Mul[i].Qj = UNDEFINED;
				RES_Stat_Mul[i].res_dest = UNDEFINED;
				RES_Stat_Mul[i].res_A = UNDEFINED;
				RES_Stat_Mul[i].op_num = UNDEFINED;
				RES_Stat_Mul[i].if_branch = UNDEFINED;
				RES_Stat_Mul[i].use_ex = 1;
				RES_Stat_Mul[i].op = NOTVALID;
				RES_Stat_Mul[i].wr_done = 1;
				flag_mul[i] = 0;
			}
	}

	unsigned if_addr_same;
	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){

		if(flag_mem[i] == 1){

			switch (opcode_map[instruction_memory[RES_Stat_Load[i].op_num].op_code]){
				case LW:
				case LWS:
					b_rob = RES_Stat_Load[i].res_dest + 1;
	
					RES_Stat_Load[i].res_busy = 0;

				
					for(unsigned j = 0; j < RES_Stat_Int.size(); j++){
						if(RES_Stat_Int[j].Qj == b_rob){
							RES_Stat_Int[j].Vj = mem_result[i];
							RES_Stat_Int[j].Qj = 0;
						}
						if(RES_Stat_Int[j].Qk == b_rob){
							RES_Stat_Int[j].Vk = mem_result[i];
							RES_Stat_Int[j].Qk = 0;
						}
					}

					for(unsigned j = 0; j < RES_Stat_Add.size(); j++){
	
						if(RES_Stat_Add[j].Qj == b_rob){
							RES_Stat_Add[j].Vj = mem_result[i];
							RES_Stat_Add[j].Qj = 0;
						}
						if(RES_Stat_Add[j].Qk == b_rob){
	
							RES_Stat_Add[j].Vk = mem_result[i];
							RES_Stat_Add[j].Qk = 0;
						}
					}

					for(unsigned j=0; j < RES_Stat_Mul.size(); j++){
						if(RES_Stat_Mul[j].Qj == b_rob){
							RES_Stat_Mul[j].Vj = mem_result[i];
							RES_Stat_Mul[j].Qj = 0;
						}
						if(RES_Stat_Mul[j].Qk == b_rob){
							RES_Stat_Mul[j].Vk = mem_result[i];
							RES_Stat_Mul[j].Qk = 0;
						}
					}

					for(unsigned j = 0; j < RES_Stat_Load.size(); j++){
						if(RES_Stat_Load[j].Qj == b_rob){

							RES_Stat_Load[j].Vj = mem_result[i];
							RES_Stat_Load[j].Qj = 0;
						}
						if(RES_Stat_Load[j].Qk == b_rob){
							RES_Stat_Load[j].Vk = mem_result[i];
              ROB_table[RES_Stat_Load[j].res_dest].str_add = RES_Stat_Load[j].Vk + RES_Stat_Load[j].res_A;
              ROB_table[RES_Stat_Load[j].res_dest].is_available = 1;
							RES_Stat_Load[j].Qk = 0;
						}
					}
	
            if(RES_Stat_Load[i].execution_unit_used!=UNDEFINED){
              ld_ex_done[RES_Stat_Load[i].execution_unit_used] = 1;
            }
            if(is_load_ex == 1){
              is_load_ex = 0;
            }
            ROB_table[RES_Stat_Load[i].res_dest].rob_val = mem_result[i];
						mem_result[i] = UNDEFINED;
						ROB_table[b_rob-1].ready = 1;
            pending_ins[RES_Stat_Load[i].res_dest].log_write = num_cycles;
						ROB_table[b_rob-1].state = WRITE_RESULT;
						RES_Stat_Load[i].res_pc = UNDEFINED;
						RES_Stat_Load[i].Vj = UNDEFINED;
						RES_Stat_Load[i].Vk = UNDEFINED;
						RES_Stat_Load[i].Qj = UNDEFINED;
						RES_Stat_Load[i].res_dest = UNDEFINED;
						RES_Stat_Load[i].res_A = UNDEFINED;
						RES_Stat_Load[i].op_num = UNDEFINED;
						RES_Stat_Load[i].if_branch = UNDEFINED;
						RES_Stat_Load[i].use_ex = 1;
						RES_Stat_Load[i].op = NOTVALID;
						RES_Stat_Load[i].wr_done = 1;
						flag_mem[i] = 0;

					break;

				case SW:
				case SWS:
					b_rob = RES_Stat_Load[i].res_dest + 1;
	
					if(RES_Stat_Load[i].Qk == 0){

						for(unsigned j = 0; j < RES_Stat_Load.size(); j++){
							if(RES_Stat_Load[j].address_raw == b_rob){
	
								RES_Stat_Load[j].address_raw = 0;
								RES_Stat_Load[j].from_forward = RES_Stat_Load[i].Vj;
								ld_step1[j] = 0;
								ld_current_exe[i] = 1;
							}
						}

            for(unsigned ld_ex = 0; ld_ex < ld_ex_done.size(); ld_ex++){
              if(ld_ex_done[ld_ex] == 1){
                ld_ex_done[ld_ex] = 0;
              }
            }


						ROB_table[b_rob - 1].rob_val = RES_Stat_Load[i].Vj;

						ROB_table[b_rob - 1].ready = 1;
            pending_ins[RES_Stat_Load[i].res_dest].log_write = num_cycles;
						ROB_table[RES_Stat_Load[i].res_dest].state = WRITE_RESULT;
						RES_Stat_Load[i].res_busy = 0;
						RES_Stat_Load[i].res_pc = UNDEFINED;
						RES_Stat_Load[i].Vj = UNDEFINED;
						RES_Stat_Load[i].Vk = UNDEFINED;
						RES_Stat_Load[i].Qj = UNDEFINED;
						RES_Stat_Load[i].res_dest = UNDEFINED;
						RES_Stat_Load[i].res_A = UNDEFINED;
						RES_Stat_Load[i].op_num = UNDEFINED;
						RES_Stat_Load[i].if_branch = UNDEFINED;
						RES_Stat_Load[i].use_ex = 1;
						RES_Stat_Load[i].op = NOTVALID;
						RES_Stat_Load[i].wr_done = 1;
						flag_mem[i] = 0;
					}
					break;
			}
		}
	}
}

void sim_ooo::commit_stage(){
	unsigned d;
	unsigned flag_end = 0;
	res_type_t type_res;
	is_cleared = 0;
	unsigned if_store_commit = 0;
	unsigned hold_mem;
  if(ROB_table[head_ROB].ready == 1){
	
		d = ROB_table[head_ROB].rob_dest;
		type_res = ROB_table[head_ROB].dest_type;

		switch(opcode_map[instruction_memory[ROB_table[head_ROB].rob_temp_pc].op_code]){
			case BEQZ:
			case BNEZ:
			case BLTZ:
			case BGTZ:
			case BLEZ:
			case BGEZ:
				num_instructions++;
        ROB_table[head_ROB].state = COMMIT;
        pending_ins[head_ROB].log_commit = num_cycles;
				if(ROB_table[head_ROB].if_branch_rob == 1){
					current_PC = ROB_table[head_ROB].rob_val;
          ROB_table[head_ROB].state = COMMIT;
          pending_ins[head_ROB].log_commit = num_cycles;
          
					if_branch = 1;
				}
				break;

			case SW:
			case SWS:
        if_store_commit = 1;
		
				for(unsigned sel_ld_unit = 0; sel_ld_unit < big_mem.size(); sel_ld_unit++){
		
						if(big_mem[sel_ld_unit].status == 1 && ROB_table[head_ROB].state == WRITE_RESULT){
							if(is_load_ex == 1){
    
								continue;
							}
							big_mem[sel_ld_unit].status = 0;
							ROB_table[head_ROB].state = COMMIT;
              pending_ins[head_ROB].log_commit = num_cycles;

							is_store_ex = 1;
							num_instructions++;
					}
				}

				for(unsigned sel_ld_unit = 0; sel_ld_unit < big_mem.size(); sel_ld_unit++){
					if(big_mem[sel_ld_unit].status == 0){
            if(is_load_ex == 0){
  
  						if(num_latency_mem[sel_ld_unit] > 1){
  							num_latency_mem[sel_ld_unit]--;
  
  						}
  						else{
                
  							unsigned2char(ROB_table[head_ROB].rob_val,data_memory+ROB_table[head_ROB].rob_dest);
  							big_mem[sel_ld_unit].status = 1;
  							ld_ex_done[sel_ld_unit] = 1;
  							for(unsigned a = 0; a < create_exe_unit.size(); a++){
  								if(create_exe_unit[a].exe_unit == MEMORY){
  									hold_mem = a;
  								}
  							}
  							if_store_commit = 0;
  							num_latency_mem[sel_ld_unit] = create_exe_unit[hold_mem].latency_exe;
  							is_store_ex = 0;
  						}
            }
					}
				}
				break;


			case XOR:
			case ADD:
			case SUB:
			case OR:
			case AND:
			case MULT:
			case DIV:
			case XORI:
			case ADDI:
			case SUBI:
			case ORI:
			case ANDI:
			case LW:
				num_instructions++;
	
				register_file[d] = ROB_table[head_ROB].rob_val;
        pending_ins[head_ROB].log_commit = num_cycles;
        ROB_table[head_ROB].state = COMMIT;
				break;

			case ADDS:
			case SUBS:
			case MULTS:
			case DIVS:
			case LWS:
				num_instructions++;
				float_reg_file[d] = unsigned2float(ROB_table[head_ROB].rob_val);
        pending_ins[head_ROB].log_commit = num_cycles;
        ROB_table[head_ROB].state = COMMIT;
				break;

			default:
				break;
		}
		if(if_store_commit == 0){
      final_log.push_back(pending_ins[head_ROB]);
			ROB_table[head_ROB].rob_busy = false;
			ROB_table[head_ROB].ready = false;
			ROB_table[head_ROB].rob_pc = UNDEFINED;
			ROB_table[head_ROB].state = INVALID;
			ROB_table[head_ROB].rob_dest = UNDEFINED;
			ROB_table[head_ROB].dest_type = NOTASSIGNED;
			ROB_table[head_ROB].if_commit = 1;
      ROB_table[head_ROB].is_available = 0;
      pending_ins[head_ROB].log_pc = UNDEFINED;
      pending_ins[head_ROB].log_issue = UNDEFINED;
      pending_ins[head_ROB].log_execute = UNDEFINED;
      pending_ins[head_ROB].log_write = UNDEFINED;
      pending_ins[head_ROB].log_commit = UNDEFINED;

			if(type_res == INTEGER_RES){
				if(int_reg_stat[d].ROB_num == head_ROB){
					int_reg_stat[d].reg_busy = 0;
          int_reg_stat[d].ROB_num = UNDEFINED;
				}
			}
			if(type_res == FLOAT_RES){
				if(float_reg_stat[d].ROB_num == head_ROB){
					float_reg_stat[d].reg_busy = 0;
          float_reg_stat[d].ROB_num = UNDEFINED;
				}
			}


			head_ROB = head_ROB + 1;


	
			if(head_ROB == ROB_table.size()){

				head_ROB = 0;
			}
		}
    
		if(if_branch == 1){

			check_end = 0;
			is_cleared = 1;
			
		}
	}
	if(check_end == 1){
		for(unsigned iter = 0; iter < ROB_table.size(); iter++){
	
				if(ROB_table[iter].rob_busy != 0){
	
					flag_end = 0;
					break;
				}
				else{
					flag_end = 1;
				}
		}
	}
	if(flag_end == 1){
			eop_end = 1;
	}
	else{
		eop_end = 0;
	}

}


void sim_ooo::clear_log(){
  for(unsigned i = (head_ROB); i < ROB_table.size(); i++){
    if(pending_ins[i].log_pc != UNDEFINED){
      final_log.push_back(pending_ins[i]);
    }
  }
  for(unsigned i = 0; i < head_ROB; i++){
    if(pending_ins[i].log_pc != UNDEFINED){
      final_log.push_back(pending_ins[i]);
    }
  }

  for(unsigned a = 0; a < pending_ins.size(); a++){
    pending_ins[a] = temp_log;
  }
}

void sim_ooo::clear_rob(){
	for(unsigned a = 0; a < ROB_table.size(); a++){           
		ROB_tab.entry = a+1;
		ROB_table[a] = ROB_tab;
		head_ROB = 0;
	}
}

void sim_ooo::clear_res(){
	RES_Temp.res_name = "Int";
	for(unsigned lint = 0; lint < RES_Stat_Int.size(); lint++){
    RES_Stat_Int[lint] = RES_Temp;
		int_result[lint] = UNDEFINED;
		flag_int[lint] = 0;
  }

	RES_Temp.res_name = "Mult";
  for(unsigned lmul = 0; lmul < RES_Stat_Mul.size(); lmul++){
		RES_Stat_Mul[lmul] = RES_Temp;
		mul_result[lmul] = UNDEFINED;
		flag_mul[lmul] = 0;
  }

	RES_Temp.res_name = "Add";
  for(unsigned ladd = 0; ladd < RES_Stat_Add.size(); ladd++){
		RES_Stat_Add[ladd] = RES_Temp;
		add_result[ladd] = UNDEFINED;
		flag_add[ladd] = 0;
	}

	RES_Temp.res_name = "Load";
  for(unsigned lload = 0; lload < RES_Stat_Load.size(); lload++){
		RES_Stat_Load[lload] = RES_Temp;
		mem_result[lload] = UNDEFINED;
		flag_mem[lload] = 0;
	}
}

void sim_ooo::clear_reg(){
	for(unsigned i = 0; i < NUM_GP_REGISTERS; i++){
    int_reg_stat[i].reg_busy = 0;
    int_reg_stat[i].ROB_num = UNDEFINED;
    float_reg_stat[i].reg_busy = 0;
    float_reg_stat[i].ROB_num = UNDEFINED;
  }
}

void sim_ooo::clear_exe(){
	unsigned hold_int;
	unsigned hold_mem;
	unsigned hold_mul;
	unsigned hold_div;
	unsigned hold_add;

	for(unsigned a = 0; a < create_exe_unit.size(); a++){
		if(create_exe_unit[a].exe_unit == INTEGER){
			hold_int = a;
		}
	}
	for(unsigned a = 0; a < create_exe_unit.size(); a++){
		if(create_exe_unit[a].exe_unit == MULTIPLIER){
			hold_mul = a;
		}
	}
	for(unsigned a = 0; a < create_exe_unit.size(); a++){
		if(create_exe_unit[a].exe_unit == DIVIDER){
			hold_div = a;
		}
	}
	for(unsigned a = 0; a < create_exe_unit.size(); a++){
		if(create_exe_unit[a].exe_unit == ADDER){
			hold_add = a;
		}
	}
	for(unsigned a = 0; a < create_exe_unit.size(); a++){
		if(create_exe_unit[a].exe_unit == MEMORY){
			hold_mem = a;
		}
	}
	for (unsigned i = 0; i < big_add.size(); i++){
		num_latency_add[i] = create_exe_unit[hold_add].latency_exe;
		big_add[i].status = 1;
		big_add[i].res_stat_num = UNDEFINED;
	}

	for (unsigned i = 0; i < big_int.size(); i++){
		num_latency_int[i] = create_exe_unit[hold_int].latency_exe;
		big_int[i].status = 1;
		big_int[i].res_stat_num = UNDEFINED;
	}

	for (unsigned i = 0; i < big_div.size(); i++){
		num_latency_div[i] = create_exe_unit[hold_div].latency_exe;
		big_div[i].status = 1;
		big_div[i].res_stat_num = UNDEFINED;
	}

	for (unsigned i = 0; i < big_mul.size(); i++){
		num_latency_mul[i] = create_exe_unit[hold_mul].latency_exe;
		big_mul[i].status = 1;
		big_mul[i].res_stat_num = UNDEFINED;
	}

	for (unsigned i = 0; i < big_mul.size(); i++){
		num_latency_mem[i] = create_exe_unit[hold_mem].latency_exe;
		big_mem[i].status = 1;
		big_mem[i].res_stat_num = UNDEFINED;
	}
}

void sim_ooo::reset(){
}

int sim_ooo::get_int_register(unsigned reg){
	return register_file[reg]; //fill here
}

void sim_ooo::set_int_register(unsigned reg, int value){
  register_file[reg] = value;
}

float sim_ooo::get_fp_register(unsigned reg){
	return float_reg_file[reg]; //fill here
}

void sim_ooo::set_fp_register(unsigned reg, float value){
  float_reg_file[reg] = value;
}

unsigned sim_ooo::get_pending_int_register(unsigned reg){
	return int_reg_stat[reg].ROB_num; //fill here
}

unsigned sim_ooo::get_pending_fp_register(unsigned reg){
	return float_reg_stat[reg].ROB_num; //fill here
}

void sim_ooo::print_status(){
	print_pending_instructions();
	print_rob();
	print_reservation_stations();
	print_registers();
}

void sim_ooo::print_memory(unsigned start_address, unsigned end_address){
	cout << "DATA MEMORY[0x" << hex << setw(8) << setfill('0') << start_address << ":0x" << hex << setw(8) << setfill('0') <<  end_address << "]" << endl;
	for (unsigned i=start_address; i<end_address; i++){
		if (i%4 == 0) cout << "0x" << hex << setw(8) << setfill('0') << i << ": ";
		cout << hex << setw(2) << setfill('0') << int(data_memory[i]) << " ";
		if (i%4 == 3){
			cout << endl;
		}
	}
}

void sim_ooo::write_memory(unsigned address, unsigned value){
	unsigned2char(value,data_memory+address);
}

void sim_ooo::print_registers(){
        unsigned i;
	cout << "GENERAL PURPOSE REGISTERS" << endl;
	cout << setfill(' ') << setw(8) << "Register" << setw(22) << "Value" << setw(5) << "ROB" << endl;
        for (i=0; i< NUM_GP_REGISTERS; i++){
                if (get_pending_int_register(i)!=UNDEFINED)
			cout << setfill(' ') << setw(7) << "R" << dec << i << setw(22) << "-" << setw(5) << get_pending_int_register(i) << endl;
                else if (get_int_register(i)!=(int)UNDEFINED)
			cout << setfill(' ') << setw(7) << "R" << dec << i << setw(11) << get_int_register(i) << hex << "/0x" << setw(8) << setfill('0') << get_int_register(i) << setfill(' ') << setw(5) << "-" << endl;
        }
	for (i=0; i< NUM_GP_REGISTERS; i++){
                if (get_pending_fp_register(i)!=UNDEFINED)
			cout << setfill(' ') << setw(7) << "F" << dec << i << setw(22) << "-" << setw(5) << get_pending_fp_register(i) << endl;
                else if (get_fp_register(i)!=UNDEFINED)
			cout << setfill(' ') << setw(7) << "F" << dec << i << setw(11) << get_fp_register(i) << hex << "/0x" << setw(8) << setfill('0') << float2unsigned(get_fp_register(i)) << setfill(' ') << setw(5) << "-" << endl;
	}
	cout << endl;
}

void sim_ooo::print_rob(){
	cout << "REORDER BUFFER" << endl;
	cout << setfill(' ') << setw(5) << "Entry" << setw(6) << "Busy" << setw(7) << "Ready" << setw(12) << "PC" << setw(10) << "State" << setw(6) << "Dest" << setw(12) << "Value" << endl;
	
	for(unsigned i = 0; i < ROB_table.size(); i++){
		std::cout << setfill(' ') << setw(5) << (i);
		if(ROB_table[i].rob_busy){std::cout << setw(6) << "yes";}
		else{std::cout << setw(6) << "no";}
		(ROB_table[i].ready)?std::cout << setw(7) << "yes":std::cout << setw(7) << "no";
		if(ROB_table[i].state == INVALID){
			std::cout << setw(12) << "-";
		}
		else{
			std::cout << setw(4) << hex << "0x" << setw(8) << setfill('0') << ROB_table[i].rob_pc;
		}
		switch (ROB_table[i].state) {
			case ISSUE:
				std::cout << setfill(' ') << setw(10) << "ISSUE";
				break;
			case EXECUTE:
				std::cout << setfill(' ') << setw(10) << "EXE";
				break;
			case WRITE_RESULT:
				std::cout << setfill(' ') << setw(10) << "WR";
				break;
			case COMMIT:
				std::cout << setfill(' ') << setw(10) << "COMMIT";
				break;
			case INVALID:
				std::cout << setfill(' ') << setw(10) << "-";
				break;
		}
		if(ROB_table[i].state == INVALID || ROB_table[i].rob_dest == UNDEFINED)std::cout << setw(6) << "-";
		else if(ROB_table[i].dest_type == FLOAT_RES)std::cout << setfill(' ') << setw(5) << "F" << ROB_table[i].rob_dest;
		else if(ROB_table[i].dest_type == INTEGER_RES)std::cout << setfill(' ') << setw(5) << "R" << ROB_table[i].rob_dest;
		else if(ROB_table[i].dest_type == STORE_DEST)std::cout << " " << dec << ROB_table[i].rob_dest;
		if(ROB_table[i].state == WRITE_RESULT || ROB_table[i].state == COMMIT)std::cout << setw(4) << setfill(' ') << "0x" << setw(8) << setfill('0') << hex << ROB_table[i].rob_val << '\n';
		else std::cout << setw(12) << "-" << '\n';
	}

	cout << endl;
}

void sim_ooo::print_reservation_stations(){
	cout << "RESERVATION STATIONS" << endl;
	cout  << setfill(' ');
	cout << setw(7) << "Name" << setw(6) << "Busy" << setw(12) << "PC" << setw(12) << "Vj" << setw(12) << "Vk" << setw(6) << "Qj" << setw(6) << "Qk" << setw(6) << "Dest" << setw(12) << "Address" << endl;


	for(unsigned i = 0; i < RES_Stat_Int.size(); i++){
		std::cout << setfill(' ') << setw(6) << RES_Stat_Int[i].res_name << (i+1);
		if(RES_Stat_Int[i].res_busy == 1)std::cout << setw(6) << "yes";
		else std::cout << setw(6) << "no";
		if(RES_Stat_Int[i].res_busy == 1)std::cout << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Int[i].res_pc;
		else std::cout << setw(12) << "-";
		if((RES_Stat_Int[i].res_busy == 1) && (RES_Stat_Int[i].Vj!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Int[i].Vj;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Int[i].res_busy == 1) && (RES_Stat_Int[i].Vk!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Int[i].Vk;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Int[i].res_busy == 1) && (RES_Stat_Int[i].Qj!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Int[i].Qj-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Int[i].res_busy == 1) && (RES_Stat_Int[i].Qk!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Int[i].Qk-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if(RES_Stat_Int[i].res_busy == 1)std::cout << setw(6) << RES_Stat_Int[i].res_dest;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Int[i].res_busy == 1) && (RES_Stat_Int[i].res_A != UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Int[i].res_A << '\n';
		else std::cout << setfill(' ') << setw(12) << "-" << '\n';
	}

	for(unsigned i = 0; i < RES_Stat_Load.size(); i++){
		std::cout << setfill(' ') << setw(6) << RES_Stat_Load[i].res_name << (i+1);
		if(RES_Stat_Load[i].res_busy == 1)std::cout << setw(6) << "yes";
		else std::cout << setw(6) << "no";
		if(RES_Stat_Load[i].res_busy == 1)std::cout << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Load[i].res_pc;
		else std::cout << setw(12) << "-";
		if((RES_Stat_Load[i].res_busy == 1) && (RES_Stat_Load[i].Vj!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Load[i].Vj;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Load[i].res_busy == 1) && (RES_Stat_Load[i].Vk!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Load[i].Vk;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Load[i].res_busy == 1) && (RES_Stat_Load[i].Qj!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Load[i].Qj-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Load[i].res_busy == 1) && (RES_Stat_Load[i].Qk!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Load[i].Qk-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if(RES_Stat_Load[i].res_busy == 1)std::cout << setw(6) << RES_Stat_Load[i].res_dest;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Load[i].res_busy == 1) && (RES_Stat_Load[i].res_A != UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Load[i].res_A << '\n';
		else std::cout << setfill(' ') << setw(12) << "-" << '\n';
	}

	for(unsigned i = 0; i < RES_Stat_Add.size(); i++){
		std::cout << setfill(' ') << setw(6) << RES_Stat_Add[i].res_name << (i+1);
		if(RES_Stat_Add[i].res_busy == 1)std::cout << setw(6) << "yes";
		else std::cout << setw(6) << "no";
		if(RES_Stat_Add[i].res_busy == 1)std::cout << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Add[i].res_pc;
		else std::cout << setw(12) << "-";
		if((RES_Stat_Add[i].res_busy == 1) && (RES_Stat_Add[i].Vj!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Add[i].Vj;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Add[i].res_busy == 1) && (RES_Stat_Add[i].Vk!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Add[i].Vk;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Add[i].res_busy == 1) && (RES_Stat_Add[i].Qj!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Add[i].Qj-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Add[i].res_busy == 1) && (RES_Stat_Add[i].Qk!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Add[i].Qk-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if(RES_Stat_Add[i].res_busy == 1)std::cout << setw(6) << RES_Stat_Add[i].res_dest;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Add[i].res_busy == 1) && (RES_Stat_Add[i].res_A != UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Add[i].res_A << '\n';
		else std::cout << setfill(' ') << setw(12) << "-" << '\n';
	}

	for(unsigned i = 0; i < RES_Stat_Mul.size(); i++){
		std::cout << setfill(' ') << setw(6) << RES_Stat_Mul[i].res_name << (i+1);
		if(RES_Stat_Mul[i].res_busy == 1)std::cout << setw(6) << "yes";
		else std::cout << setw(6) << "no";
		if(RES_Stat_Mul[i].res_busy == 1)std::cout << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Mul[i].res_pc;
		else std::cout << setw(12) << "-";
		if((RES_Stat_Mul[i].res_busy == 1) && (RES_Stat_Mul[i].Vj!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Mul[i].Vj;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Mul[i].res_busy == 1) && (RES_Stat_Mul[i].Vk!=UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Mul[i].Vk;
		else std::cout << setfill(' ') << setw(12) << "-";
		if((RES_Stat_Mul[i].res_busy == 1) && (RES_Stat_Mul[i].Qj!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Mul[i].Qj-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Mul[i].res_busy == 1) && (RES_Stat_Mul[i].Qk!=0))std::cout << setfill(' ') << setw(6) << RES_Stat_Mul[i].Qk-1;
		else std::cout << setfill(' ') << setw(6) << "-";
		if(RES_Stat_Mul[i].res_busy == 1)std::cout << setw(6) << RES_Stat_Mul[i].res_dest;
		else std::cout << setfill(' ') << setw(6) << "-";
		if((RES_Stat_Mul[i].res_busy == 1) && (RES_Stat_Mul[i].res_A != UNDEFINED))std::cout << setfill(' ') << setw(4) << hex << "0x" << setw(8) << setfill('0') << RES_Stat_Mul[i].res_A << '\n';
		else std::cout << setfill(' ') << setw(12) << "-" << '\n';
	}

	cout << endl;
}

void sim_ooo::print_pending_instructions(){
	cout << "PENDING INSTRUCTIONS STATUS" << endl;
	cout << setfill(' ');
	cout << setw(10) << "PC" << setw(7) << "Issue" << setw(7) << "Exe" << setw(7) << "WR" << setw(7) << "Commit" << '\n';
  for(unsigned i = 0; i < pending_ins.size(); i++){
    if(pending_ins[i].log_pc != UNDEFINED){
      std::cout << hex << "0x" << setw(8) << setfill('0') << pending_ins[i].log_pc << setfill(' ');
      if(pending_ins[i].log_issue!=UNDEFINED) std::cout << dec << setw(7) << pending_ins[i].log_issue; else std::cout << setw(7) << "-";
      if(pending_ins[i].log_execute!=UNDEFINED) std::cout << dec << setw(7) << pending_ins[i].log_execute; else std::cout << setw(7) << "-";
      if(pending_ins[i].log_write!=UNDEFINED) std::cout << dec << setw(7) << pending_ins[i].log_write; else std::cout << setw(7) << "-";
      if(pending_ins[i].log_commit!=UNDEFINED) std::cout << dec << setw(7) << pending_ins[i].log_commit << '\n'; else std::cout << setw(7) << "-" << '\n';
    }
    else{
      std::cout << setw(10) << "-" << setw(7) << "-" << setw(7) << "-" << setw(7) << "-" << setw(7) << "-" << '\n';
    }
  }

	cout << endl;
}

void sim_ooo::print_log(){
  std::cout << "EXECUTION LOG" << '\n';
  cout << setfill(' ');
  cout << setw(10) << "PC" << setw(7) << "Issue" << setw(7) << "Exe" << setw(7) << "WR" << setw(7) << "Commit" << '\n';
  for(unsigned i = 0; i < final_log.size(); i++){
    if(final_log[i].log_pc != UNDEFINED){
      std::cout << hex << "0x" << setw(8) << setfill('0') << final_log[i].log_pc << setfill(' ');
      if(final_log[i].log_issue!=UNDEFINED) std::cout << dec << setw(7) << final_log[i].log_issue; else std::cout << setw(7) << "-";
      if(final_log[i].log_execute!=UNDEFINED) std::cout << dec << setw(7) << final_log[i].log_execute; else std::cout << setw(7) << "-";
      if(final_log[i].log_write!=UNDEFINED) std::cout << dec << setw(7) << final_log[i].log_write; else std::cout << setw(7) << "-";
      if(final_log[i].log_commit!=UNDEFINED) std::cout << dec << setw(7) << final_log[i].log_commit << '\n'; else std::cout << setw(7) << "-" << '\n';
    }
  }
}

float sim_ooo::get_IPC(){
	return float(float(num_instructions)/float(num_cycles)); 
}

unsigned sim_ooo::get_instructions_executed(){
	return  num_instructions; 
}

unsigned sim_ooo::get_clock_cycles(){
	return num_cycles; 
}



int main(int argc, char **argv){

	unsigned i;

	
	sim_ooo *ooo = new sim_ooo(1024*1024,	//memory size
				   6,           //rob size
				   1, 2, 2, 2); //int, add, mult, load reservation stations

	//initialize execution units
        ooo->init_exec_unit(INTEGER, 2, 1);
        ooo->init_exec_unit(ADDER, 2, 2);
        ooo->init_exec_unit(MULTIPLIER, 10, 1);
        ooo->init_exec_unit(DIVIDER, 40, 1);
        ooo->init_exec_unit(MEMORY, 1, 1);

	//loads program in instruction memory at address 0x00000000
	ooo->load_program("asm/code_ooo.asm", 0x00000000);

	//initialize general purpose registers
	ooo->set_int_register(1, 10);
	ooo->set_int_register(2, 20);
	ooo->set_int_register(3, 10);
	for (i=0; i<11; i++) ooo->set_fp_register(i, (float)i*10.0);

	//initialize data memory and prints its content (for the specified address ranges)
	ooo->write_memory(0x14,float2unsigned(10.0));
	ooo->write_memory(0x28,float2unsigned(30.0));

	cout << "\nBEFORE PROGRAM EXECUTION..." << endl;
	cout << "======================================================================" << endl << endl;

	//prints the value of the memory and registers
	ooo->print_registers();
	ooo->print_memory(0x0, 0x30);

	// executes the program
	cout << "\n*****************************" << endl;
	cout << "STARTING THE PROGRAM..." << endl;
	cout << "*****************************" << endl << endl;

	// first 20 clock cycles
	cout << "First 20 clock cycles: inspecting the registers at each clock cycle..." << endl;
	cout << "======================================================================" << endl << endl;

	for (i=0; i<20; i++){
		cout << "CLOCK CYCLE #" << dec << i << endl;
		ooo->run(1);
		ooo->print_status();
		cout << endl;
	}

	// runs program to completion
	cout << "EXECUTING PROGRAM TO COMPLETION..." << endl << endl;
	ooo->run();

	cout << "PROGRAM TERMINATED\n";
	cout << "===================" << endl << endl;

	//prints the value of registers and data memory
	ooo->print_status();
	ooo->print_memory(0x0, 0x30);
	cout << endl;

	//print the execution log
	ooo->print_log();

	cout << endl;

	// prints the number of instructions executed and IPC
	cout << "Instruction executed = " << dec << ooo->get_instructions_executed() << endl;
	cout << "Clock cycles = " << dec << ooo->get_clock_cycles() << endl;
	cout << "IPC = " << dec << ooo->get_IPC() << endl;
}
