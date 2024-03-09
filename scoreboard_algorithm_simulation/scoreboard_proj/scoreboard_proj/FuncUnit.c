#include "FuncUnit.h"

void InitUnit(Unit* unit, int index, char* name, int unit_type) {
	//unit type
	unit->unit_type = unit_type;
	//unit name
	char name_copy[8];
	strcpy(name_copy, name);
	char index_num_str[5];
	_itoa(index, index_num_str, 10);
	strcat(name_copy, index_num_str);
	strcpy(unit->name, name_copy);
	unit->pc_inst = -1;
	//reset all other fields
	ResetUnit(unit);
}

void ResetUnit(Unit* unit) {
	unit->busy = 0;
	unit->issue_cycle = 0;
	unit->exec_cycles_left = 0;
	unit->exec_started = false;
	unit->res = 0;
	unit->pc_inst = -1;
	strcpy(unit->Fi, "-");
	strcpy(unit->Fj, "-");
	strcpy(unit->Fk, "-");
	strcpy(unit->Qj, "-");
	strcpy(unit->Qk, "-");
	strcpy(unit->Rj, "-");
	strcpy(unit->Rk, "-");
}

Unit* CreateUnitArray(int num_of_units, char* name, int unit_type) {

	Unit* unit_arr_cp = (Unit*)malloc(num_of_units * sizeof(Unit));
	int i = 0;
	if (unit_arr_cp != NULL)
	{
		for (i; i < num_of_units; i++) {
			InitUnit(&unit_arr_cp[i], i, name, unit_type);
		}
	}
	return unit_arr_cp;
}

void CreateUnitsTable(UnitsTable* units_table, UserConf* user_conf) {
	Unit* add_unit_arr, * sub_unit_arr, * mul_unit_arr, * div_unit_arr, * ld_unit_arr, * st_unit_arr;
	strcpy(units_table->trace_unit, user_conf->trace_unit);
	units_table->trace_unit[strlen(units_table->trace_unit) - 1] = '\0';
	add_unit_arr = CreateUnitArray(user_conf->add_nr_units, "ADD", ADD);
	sub_unit_arr = CreateUnitArray(user_conf->sub_nr_units, "SUB", SUB);
	mul_unit_arr = CreateUnitArray(user_conf->mul_nr_units, "MUL", MUL);
	div_unit_arr = CreateUnitArray(user_conf->div_nr_units, "DIV", DIV);
	ld_unit_arr = CreateUnitArray(user_conf->ld_nr_units, "LD", LD);
	st_unit_arr = CreateUnitArray(user_conf->st_nr_units, "ST", ST);
	units_table->add_units_array = add_unit_arr;
	units_table->add_nr_units = user_conf->add_nr_units;
	units_table->add_delay = user_conf->add_delay;
	units_table->sub_units_array = sub_unit_arr;
	units_table->sub_nr_units = user_conf->sub_nr_units;
	units_table->sub_delay = user_conf->sub_delay;
	units_table->mul_units_array = mul_unit_arr;
	units_table->mul_nr_units = user_conf->mul_nr_units;
	units_table->mul_delay = user_conf->mul_delay;
	units_table->div_units_array = div_unit_arr;
	units_table->div_nr_units = user_conf->div_nr_units;
	units_table->div_delay = user_conf->div_delay;
	units_table->ld_units_array = ld_unit_arr;
	units_table->ld_nr_units = user_conf->ld_nr_units;
	units_table->ld_delay = user_conf->ld_delay;
	units_table->st_units_array = st_unit_arr;
	units_table->st_nr_units = user_conf->st_nr_units;
	units_table->st_delay = user_conf->st_delay;
	units_table->total_nr_units = user_conf->add_nr_units + user_conf->sub_nr_units + user_conf->mul_nr_units + user_conf->div_nr_units + user_conf->ld_nr_units + user_conf->st_nr_units;
}

void FreeUnitsTable(UnitsTable* unit_table) {
	free(unit_table->add_units_array);
	free(unit_table->sub_units_array);
	free(unit_table->mul_units_array);
	free(unit_table->div_units_array);
	free(unit_table->ld_units_array);
	free(unit_table->st_units_array);
}

int search_free_unit(Unit* units_array, int type, int size) { //returns -1 if none free unit, and index if found
	int i;
	for (i = 0; i < size; i++) {
		if (!units_array[i].busy)
			return i;
	}
	return -1;
}

int get_free_unit(InstQueue* inst_queue, UnitsTable* units_table) {
	int free_unit_index = -1;
	switch (current_opcode(inst_queue))
	{
	case ADD:
		free_unit_index = search_free_unit(units_table->add_units_array, ADD, units_table->add_nr_units);
		break;
	case SUB:
		free_unit_index = search_free_unit(units_table->sub_units_array, SUB, units_table->sub_nr_units);
		break;
	case MUL:
		free_unit_index = search_free_unit(units_table->mul_units_array, MUL, units_table->mul_nr_units);
		break;
	case DIV:
		free_unit_index = search_free_unit(units_table->div_units_array, DIV, units_table->div_nr_units);
		break;
	case LD:
		free_unit_index = search_free_unit(units_table->ld_units_array, LD, units_table->ld_nr_units);
		break;
	case ST:
		free_unit_index = search_free_unit(units_table->st_units_array, ST, units_table->st_nr_units);
		break;
	case HALT:
		free_unit_index = -2;
		break;
	default:
		break;
	}

	return free_unit_index;
}

void update_unit_reg_name(Unit* unit, int reg_num,char *unit_reg) {
	char reg_num_str[3];
	char reg_name[5] = "F";
	_itoa(reg_num, reg_num_str, 10);
	strcat(reg_name, reg_num_str);
	if (!strcmp(unit_reg,"Fi"))
		strcpy(unit->Fi, reg_name);
	if (!strcmp(unit_reg, "Fj"))
		strcpy(unit->Fj, reg_name);
	if (!strcmp(unit_reg, "Fk"))
		strcpy(unit->Fk, reg_name);
}

// model a buffer which passes D table to Q table on every end of cycle;
void unit_NS_update(UnitsTable *D, UnitsTable* Q) {
	int i ;
	//add units
	for (i = 0; i < D->add_nr_units; i++) {
		unit_copy(&Q->add_units_array[i], &D->add_units_array[i]);
	}
	//sub units
	for (i = 0; i < D->sub_nr_units; i++) {
		unit_copy(&Q->sub_units_array[i], &D->sub_units_array[i]);
	}
	//mul units
	for (i = 0; i < D->mul_nr_units; i++) {
		unit_copy(&Q->mul_units_array[i], &D->mul_units_array[i]);
	}
	//div units
	for (i = 0; i < D->div_nr_units; i++) {
		unit_copy(&Q->div_units_array[i], &D->div_units_array[i]);
	}
	//ld units
	for (i = 0; i < D->ld_nr_units; i++) {
		unit_copy(&Q->ld_units_array[i], &D->ld_units_array[i]);
	}
	//st units
	for (i = 0; i < D->st_nr_units; i++) {
		unit_copy(&Q->st_units_array[i], &D->st_units_array[i]);
	}

}

void unit_copy(Unit* Dest, Unit* Src) {
	Dest->busy = Src->busy;
	Dest->issue_cycle = Src->issue_cycle;
	Dest->exec_cycles_left = Src->exec_cycles_left;
	Dest->exec_started = Src->exec_started;
	Dest->res = Src->res;
	Dest->imm = Src->imm;
	Dest->pc_inst = Src->pc_inst;
	strcpy(Dest->Fi,Src->Fi);
	strcpy(Dest->Fj, Src->Fj);
	strcpy(Dest->Fk, Src->Fk);
	strcpy(Dest->Qj, Src->Qj);
	strcpy(Dest->Qk, Src->Qk);
	strcpy(Dest->Rj, Src->Rj);
	strcpy(Dest->Rk, Src->Rk);
}

float exec_add(float data0, float data1)
{
	return (data0 + data1);
}

float exec_sub(float data0, float data1)
{
	return (data0 - data1);
}

float exec_mul(float data0, float data1)
{
	return (data0 * data1);
}

float exec_div(float data0, float data1)
{
	return (data0 / data1);
}

float exec_ld(int imm, Mem* mem)
{
	return(mem->mem_data[imm].f);

}

void exec_st(float data, int imm, Mem* mem)
{
	mem->mem_data[imm].f = data;
}

bool is_unit_busy(Unit* unit)
{
	return (unit->busy);
}

// update the relevant units if they are waiting to a unit to write back
void perform_wb_on_other_unit(Unit* me, Unit* Q_other_unit, Unit* D_other_unit, Reg* regs) 
{
	//if got myself, don't care
	if (!strcmp(me->name, Q_other_unit->name)) return;
	// if other unit's Qj is me - update Rj, Qj
	if (!strcmp(D_other_unit->Qj, me->name))
	{
		//using D - bypass for issue & WB to a register on the same cycle
		strcpy(D_other_unit->Rj, "Yes"); 
		strcpy(D_other_unit->Qj, "-");
	}
	// if other unit's Qk is me - update Rk, Qk
	if (!strcmp(D_other_unit->Qk, me->name))
	{
		//using D - bypass for issue & WB to a register on the same cycle
		strcpy(D_other_unit->Rk, "Yes");
		strcpy(D_other_unit->Qk, "-");
	}
}

// deals with WAR hazard 
bool wait_other_unit_ready_for_wb(Unit* me, Unit* Q_other_unit){
	//if got myself, don't care
	if (!strcmp(me->name, Q_other_unit->name)) return 1; 
	//if other unit is not busy, it's not interesting
	if (!(Q_other_unit->busy)) return 1;
	// wait until ((other unit's Fj != my Fi) || (other unit's Rj == "No")
	if (!(strcmp(Q_other_unit->Fj, me->Fi) || !strcmp(Q_other_unit->Rj, "No"))) return 0;
	// wait until ((other unit's Fk != my Fi) || (other unit's Rk == "No")
	if (!(strcmp(Q_other_unit->Fk, me->Fi)|| !strcmp(Q_other_unit->Rk, "No"))) return 0;
	return 1;
}

//wait for all units to deal with WAR hazard
bool wait_unit_type_ready_for_wb(int nr_units, Unit* Q_units_array, Unit* me)
{
	bool unit_type_ready_for_wb = 1;
	for (int i = 0; i < nr_units; i++)
	{
		unit_type_ready_for_wb = unit_type_ready_for_wb && wait_other_unit_ready_for_wb(me, &Q_units_array[i]);
	}
	return (unit_type_ready_for_wb);
}

//iterates over all the other units need to be updated
void perform_wb_on_unit_type(int nr_units, Unit* Q_units_array, Unit* D_units_array, Unit* me, Reg* regs)
{
	for (int i = 0; i < nr_units; i++)
	{
		perform_wb_on_other_unit(me, &Q_units_array[i], &D_units_array[i], regs);
	}
}

//checks if theres and older store command to same address which still executing 
bool check_ready_for_load(Unit* LD_unit, Unit* ST_units_array, int st_nr_units)
{
	bool ready = 1;
	//for each st_unit:
	for (int i = 0; i < st_nr_units; i++)
	{
		if ((ST_units_array[i].imm == LD_unit->imm) && (ST_units_array[i].issue_cycle < LD_unit->issue_cycle) && ST_units_array[i].exec_cycles_left > 0)
		{
			return 0; //can't load if an older op needs to store first to the same address
		}
	 }
	return 1;
}

//checks if theres and older store/load command to same address which still executing 
bool check_ready_for_store(Unit* ST_unit, Unit* ST_units_array, Unit* LD_units_array, int st_nr_units, int ld_nr_units)
{
	bool ready = 1;
	for (int i = 0; i < st_nr_units; i++)
	{
		if (!strcmp(ST_units_array[i].name, ST_unit->name)) continue; //not checking on myself
		if ((ST_units_array[i].imm == ST_unit->imm) && (ST_units_array[i].issue_cycle < ST_unit->issue_cycle) && ST_units_array[i].exec_cycles_left > 0)
		{
			return 0; //can't store if an older op needs to store first to the same address
		}
	}
	for (int i = 0; i < ld_nr_units; i++)
	{
		if ((LD_units_array[i].imm == ST_unit->imm) && (LD_units_array[i].issue_cycle < ST_unit->issue_cycle) && LD_units_array[i].exec_cycles_left > 0)
		{
			return 0; //can't store if an older op needs to load first from the same address
		}
	}
	return 1;
}