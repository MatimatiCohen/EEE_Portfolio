
#ifndef FUNCUNIT_H
#define FUNCUNIT_H

#include "utils.h"
#include "InstQueue.h"

struct Unit { // unit by definition + flags and other data to manage the subroutine steps
	int busy; //flag to check if unit is busy
	int issue_cycle;
	char name[5]; //unit name 
	char Fi[5];
	char Fj[5];
	char Fk[5];
	char Qj[5];
	char Qk[5];
	char Rj[5];
	char Rk[5];
	// more fields for results, flags and logs
	int exec_cycles_left;
	bool exec_started;
	int unit_type;
	float res;
	int imm;
	int pc_inst;
};
typedef struct Unit Unit;


struct UnitsTable { // a table of all units arrays
	Unit* add_units_array;
	Unit* sub_units_array;
	Unit* mul_units_array;
	Unit* div_units_array;
	Unit* ld_units_array;
	Unit* st_units_array;
	int add_nr_units;
	int sub_nr_units;
	int mul_nr_units;
	int div_nr_units;
	int ld_nr_units;
	int st_nr_units;
	int add_delay;
	int sub_delay;
	int mul_delay;
	int div_delay;
	int ld_delay;
	int st_delay;
	int total_nr_units;
	char trace_unit[MAX_CONF_LINE_LEN];
};
typedef struct UnitsTable UnitsTable;

//Unit functions
//General
void InitUnit(Unit* unit, int index, char* name, int unit_type);
void ResetUnit(Unit* unit);
Unit* CreateUnitArray(int num_of_units, char* name, int unit_type);
void CreateUnitsTable(UnitsTable* units_table, UserConf* user_conf);
void FreeUnitsTable(UnitsTable* unit_table);
void unit_NS_update(UnitsTable* D, UnitsTable* Q);
void unit_copy(Unit* Dest, Unit* Src);

// issue
int search_free_unit(Unit* units_array, int type, int size);
int get_free_unit(InstQueue* inst_queue, UnitsTable* units_table);
void update_unit_reg_name(Unit* unit, int reg_num, char* unit_reg);

//read operands and exec
bool is_unit_busy(Unit* unit);
bool check_ready_for_store(Unit* ST_unit, Unit* ST_units_array, Unit* LD_units_array, int st_nr_units, int ld_nr_units);
bool check_ready_for_load(Unit* LD_unit, Unit* ST_units_array, int st_nr_units);
float exec_add(float data0, float data1);
float exec_sub(float data0, float data1);
float exec_mul(float data0, float data1);
float exec_div(float data0, float data1);
float exec_ld(int imm, Mem* mem);
void exec_st(float data, int imm, Mem* mem);

// write result
void perform_wb_on_other_unit(Unit* me, Unit* Q_other_unit, Unit* D_other_unit, Reg* regs);
bool wait_other_unit_ready_for_wb(Unit* me, Unit* Q_other_unit);
bool wait_unit_type_ready_for_wb(int nr_units, Unit* Q_units_array, Unit* me);
void perform_wb_on_unit_type(int nr_units, Unit* Q_units_array, Unit* D_units_array, Unit* me, Reg* regs);

#endif