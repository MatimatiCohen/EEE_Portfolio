
#ifndef ROUTINE_H
#define ROUTINE_H


#include "utils.h"
#include "FuncUnit.h"
#include "InstQueue.h"


struct Hazard {
	int WAW;
	int RAW;

};
typedef struct Hazard Hazard;


// one instrucion data for trace
struct InstState {
	uint32_t inst;
	int pc;
	char unit[5];
	int cycle_fetched;
	int cycle_issued;
	int cycle_read_operands;
	int cycle_execute_end;
	int cycle_write_result;
};
typedef struct InstState InstState;

struct SimStatus
{
	int clock_cycle;
	int pc;
	int halt;
	int all_units_done;
	int last_inst;
	Hazard flag;
	InstState insttrace[MEM_LEN]; //array of traceable instructions
	
};
typedef struct SimStatus SimStatus;


//Pure Routine Functions
void InitSimStatus(SimStatus* sim_stauts);
void Fetch(InstQueue* inst_queue, Mem* memin,SimStatus *sim_stauts);
void Issue(InstQueue* inst_queue, UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* registers, SimStatus* sim_status);
void ReadOpsAndExec(UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* regs, Mem* mem,SimStatus *sim_status, FILE* traceunit_file);
void WriteBack(UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* regs, SimStatus* sim_status);
bool CheckAllUnitsDone(UnitsTable* units_table);
void print_instrace(char* path, SimStatus* sim_status);

//Shared Routine and Functional Units functions
void update_unit_fields(Unit* unit, Inst* inst_to_issue, Reg* registers, SimStatus* sim_status, int unit_delay);
void issue_to_unit(Inst* inst_to_issue, int unit_type, int free_unit_index, UnitsTable* units_table, Reg* registers, SimStatus* sim_status); 
bool unit_write_back(UnitsTable* D_units_table, UnitsTable* Q_units_table, Unit* me, Reg* regs);
void UnitTypeWriteBack(int nr_units, Unit* Q_units_array, Unit* D_units_array, UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* regs, SimStatus* sim_status);
void unit_read_ops_and_exec(Unit* D_unit, Unit* Q_unit, Reg* regs, Mem* mem, Unit* ST_units_array, Unit* LD_units_array, int nr_st_units, int nr_ld_units, SimStatus* sim_status);
int WAW(InstQueue* inst_queue, Reg* registers);
void traceunit(Unit* chosen_unit, SimStatus* sim_status, FILE* traceunit_file);
bool is_chosen_unit(Unit* unit, char* chosen_unit);
void init_inst_stat(InstState* stat);
#endif