#include "Routine.h"
#include <stdint.h>

void InitSimStatus(SimStatus* sim_stauts) {
	sim_stauts->clock_cycle = 0;
	sim_stauts->pc = 0;
	sim_stauts->halt = 0;
	sim_stauts->all_units_done = 0;
	sim_stauts->flag.RAW = 0;
	sim_stauts->flag.WAW = 0;
	sim_stauts->last_inst = 0;
	for (int i = 0; i < MEM_LEN; i++) {
		init_inst_stat(&sim_stauts->insttrace[i]);
	}
}

void init_inst_stat(InstState *stat) {
	stat->cycle_execute_end = -1;
	stat->cycle_issued = -1;
	stat->cycle_read_operands = -1;
	stat->cycle_write_result = -1;
	stat->inst = -1;
	stat->pc = -1;
	stat->cycle_fetched = -1;
	strcpy(stat->unit, "--");
}

void Fetch(InstQueue* inst_queue, Mem* mem, SimStatus *sim_status) {
	Inst instruction;
	//create a new instruction from memory line
	CreateInst(&instruction, mem->mem_data[sim_status->pc].u, sim_status->pc);
	if (instruction.op == HALT) {
		sim_status->last_inst = 1;
	}
	//push the instruction to the top of the queue
	enQueue(inst_queue, &instruction);
	if (instruction.op != HALT) { //if not halt - trace instruction
		sim_status->insttrace[sim_status->pc].pc = instruction.pc; //keep tracing instrutions
		sim_status->insttrace[sim_status->pc].cycle_fetched = sim_status->clock_cycle;
		sim_status->insttrace[sim_status->pc].inst = mem->mem_data[sim_status->pc].u;
	}
	
}

void Issue(InstQueue* inst_queue, UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* registers,SimStatus *sim_status) {
	int free_unit_index;
	Inst inst_to_issue;
	
	//Decode+Issue
	//check for WAW Hazard
	int dst_reg = inst_queue->instructions[inst_queue->front].dst;
		if (WAW(inst_queue, registers)) {
			sim_status->flag.WAW = 1;
			return; //if WAW don't start the issue
		}
		else {
			sim_status->flag.WAW = 0;
		}

	//check for an availible functional unit.
	//if the type required units are all taken, wait with issue (to keep in order issue).
	free_unit_index = get_free_unit(inst_queue, Q_units_table);
	if (free_unit_index==-1) {
		return;
	}
	if (free_unit_index == -2) { // Check: if we got opcode=6 it's a halt command and it won't be issued
		sim_status->halt = 1;
		return;
	}
	//issue
	int op = current_opcode(inst_queue);

	//dequeue the instruction from queue to a unit
	inst_to_issue = deQueue(inst_queue);
	//issue to a free unit
	issue_to_unit(&inst_to_issue, op, free_unit_index, D_units_table, registers, sim_status);
}

//WAW HAZARD: if an instruction wants to write to a register while there's an older instrutcion that wrote to the same register ,but hasn't finished yet, we need to hold/wait the issue. (in order)
// The way to inforce it is to wait that the dst register status won't have a tag , only value.
int WAW(InstQueue* inst_queue, Reg* registers) {
	int dst_reg = inst_queue->instructions[inst_queue->front].dst; 
	
	if(!strcmp(registers[dst_reg].tag, "--")) //if tag is empty no hazard, return 0
		return 0;
	return 1;
}

// iterates for the correct unit to issue to by opcode and free unit index
void issue_to_unit(Inst *inst_to_issue,int unit_type,int free_unit_index, UnitsTable* units_table, Reg* registers,SimStatus* sim_status) {
	int chosen_unit_index = -1;
	switch (unit_type)
	{
	case ADD:
		update_unit_fields(&units_table->add_units_array[free_unit_index], inst_to_issue, registers, sim_status, units_table->add_delay);
		
		break;
	case SUB:
		update_unit_fields(&units_table->sub_units_array[free_unit_index], inst_to_issue, registers, sim_status, units_table->sub_delay);
		break;
	case MUL:
		update_unit_fields(&units_table->mul_units_array[free_unit_index], inst_to_issue, registers, sim_status, units_table->mul_delay);
		break;
	case DIV:
		update_unit_fields(&units_table->div_units_array[free_unit_index], inst_to_issue, registers, sim_status, units_table->div_delay);
		break;
	case LD:
		update_unit_fields(&units_table->ld_units_array[free_unit_index], inst_to_issue, registers, sim_status, units_table->ld_delay);
		break;
	case ST:
		update_unit_fields(&units_table->st_units_array[free_unit_index], inst_to_issue, registers, sim_status, units_table->st_delay);
		break;
	default:
		break;
	}
}

// puts the instruction into the unit 
void update_unit_fields(Unit* unit, Inst* inst_to_issue,Reg *registers, SimStatus* sim_status, int unit_delay) {
	unit->busy = 1;
	unit->issue_cycle = sim_status->clock_cycle;
	sim_status->insttrace[inst_to_issue->pc].cycle_issued = unit->issue_cycle;
	strcpy(sim_status->insttrace[inst_to_issue->pc].unit , unit->name);
	unit->exec_cycles_left = unit_delay;
	unit->imm = inst_to_issue->imm;
	unit->pc_inst = inst_to_issue->pc;
	strcpy(registers[inst_to_issue->dst].tag,unit->name); //update dest regiter tag to unit name
	update_unit_reg_name(unit, inst_to_issue->dst, "Fi"); //update unit dest reg name
	update_unit_reg_name(unit, inst_to_issue->src0, "Fj"); //update unit src0 reg name
	update_unit_reg_name(unit, inst_to_issue->src1, "Fk"); //update unit src1 reg name
	if (!RegHasTag(registers[inst_to_issue->src0]) || !strcmp(unit->name, registers[inst_to_issue->src0].tag)) { // update Rj and Qj
		strcpy(unit->Rj, "Yes");
	}
	else {
		strcpy(unit->Rj, "No");
		strcpy(unit->Qj, registers[inst_to_issue->src0].tag);
	}
	if (!RegHasTag(registers[inst_to_issue->src1]) || !strcmp(unit->name, registers[inst_to_issue->src1].tag)) { // update Rk and Qk
		strcpy(unit->Rk, "Yes");
	}
	else {
		strcpy(unit->Rk, "No");
		strcpy(unit->Qk, registers[inst_to_issue->src1].tag);
	}
}

// perform read and execute to all of the units
void ReadOpsAndExec(UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* regs, Mem* mem,SimStatus *sim_status, FILE* traceunit_file) {
	bool busy = 0;
	bool is_chosen = 0;

	//for each unit type, iterates over all of the units
	for (int i = 0; i < Q_units_table->add_nr_units; i++)
	{
		unit_read_ops_and_exec(&D_units_table->add_units_array[i], &Q_units_table->add_units_array[i], regs, mem, Q_units_table->st_units_array, Q_units_table->ld_units_array, Q_units_table->st_nr_units, Q_units_table->ld_nr_units, sim_status);
		is_chosen = is_chosen_unit(&Q_units_table->add_units_array[i], Q_units_table->trace_unit);
		if (is_chosen) { //is this unit chose to be trace, update traceunit.txt on the fly
			busy = is_unit_busy(&Q_units_table->add_units_array[i]);
			if (busy)
			traceunit(&Q_units_table->add_units_array[i], sim_status, traceunit_file);
		}
	}
	for (int i = 0; i < Q_units_table->sub_nr_units; i++)
	{
		unit_read_ops_and_exec(&D_units_table->sub_units_array[i], &Q_units_table->sub_units_array[i], regs, mem, Q_units_table->st_units_array, Q_units_table->ld_units_array, Q_units_table->st_nr_units, Q_units_table->ld_nr_units, sim_status);
		is_chosen = is_chosen_unit(&Q_units_table->sub_units_array[i], Q_units_table->trace_unit);
		if (is_chosen) {
			busy = is_unit_busy(&Q_units_table->sub_units_array[i]);
			if (busy)
				traceunit(&Q_units_table->sub_units_array[i], sim_status, traceunit_file);
		}
	}
	for (int i = 0; i < Q_units_table->mul_nr_units; i++)
	{
		unit_read_ops_and_exec(&D_units_table->mul_units_array[i], &Q_units_table->mul_units_array[i], regs, mem, Q_units_table->st_units_array, Q_units_table->ld_units_array, Q_units_table->st_nr_units, Q_units_table->ld_nr_units, sim_status);
		is_chosen = is_chosen_unit(&Q_units_table->mul_units_array[i], Q_units_table->trace_unit);
		if (is_chosen) {
			busy = is_unit_busy(&Q_units_table->mul_units_array[i]);
			if (busy)
				traceunit(&Q_units_table->mul_units_array[i], sim_status, traceunit_file);
		}
	}
	for (int i = 0; i < Q_units_table->div_nr_units; i++)
	{
		unit_read_ops_and_exec(&D_units_table->div_units_array[i], &Q_units_table->div_units_array[i], regs, mem, Q_units_table->st_units_array, Q_units_table->ld_units_array, Q_units_table->st_nr_units, Q_units_table->ld_nr_units, sim_status);
		is_chosen = is_chosen_unit(&Q_units_table->div_units_array[i], Q_units_table->trace_unit);
		if (is_chosen) {
			busy = is_unit_busy(&Q_units_table->div_units_array[i]);
			if (busy)
				traceunit(&Q_units_table->div_units_array[i], sim_status, traceunit_file);
		}
	}
	for (int i = 0; i < Q_units_table->ld_nr_units; i++)
	{
		unit_read_ops_and_exec(&D_units_table->ld_units_array[i], &Q_units_table->ld_units_array[i], regs, mem, Q_units_table->st_units_array, Q_units_table->ld_units_array, Q_units_table->st_nr_units, Q_units_table->ld_nr_units, sim_status);
		is_chosen = is_chosen_unit(&Q_units_table->ld_units_array[i], Q_units_table->trace_unit);
		if (is_chosen) {
			busy = is_unit_busy(&Q_units_table->ld_units_array[i]);
			if (busy)
				traceunit(&Q_units_table->ld_units_array[i], sim_status, traceunit_file);
		}
	}
	for (int i = 0; i < Q_units_table->st_nr_units; i++)
	{
		unit_read_ops_and_exec(&D_units_table->st_units_array[i], &Q_units_table->st_units_array[i], regs, mem, Q_units_table->st_units_array, Q_units_table->ld_units_array, Q_units_table->st_nr_units, Q_units_table->ld_nr_units, sim_status);
		is_chosen = is_chosen_unit(&Q_units_table->st_units_array[i], Q_units_table->trace_unit);
		if (is_chosen) {
			busy = is_unit_busy(&Q_units_table->st_units_array[i]);
			if (busy)
				traceunit(&Q_units_table->st_units_array[i], sim_status, traceunit_file);
		}
	}
}

//if all units are not busy all units are done
bool CheckAllUnitsDone(UnitsTable* units_table) { 
	bool all_done = 1;
	for (int i = 0; i < units_table->add_nr_units; i++)
	{
		all_done = all_done && (!is_unit_busy(&units_table->add_units_array[i]));
	}
	for (int i = 0; i < units_table->sub_nr_units; i++)
	{
		all_done = all_done && (!is_unit_busy(&units_table->sub_units_array[i]));
	}
	for (int i = 0; i < units_table->mul_nr_units; i++)
	{
		all_done = all_done && (!is_unit_busy(&units_table->mul_units_array[i]));
	}
	for (int i = 0; i < units_table->div_nr_units; i++)
	{
		all_done = all_done && (!is_unit_busy(&units_table->div_units_array[i]));
	}
	for (int i = 0; i < units_table->ld_nr_units; i++)
	{
		all_done = all_done && (!is_unit_busy(&units_table->ld_units_array[i]));
	}
	for (int i = 0; i < units_table->st_nr_units; i++)
	{
		all_done = all_done && (!is_unit_busy(&units_table->st_units_array[i]));
	}
	return all_done;
}

//for a given unit type, perform write back, and if finished, reset unit.
void UnitTypeWriteBack(int nr_units, Unit* Q_units_array, Unit* D_units_array, UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* regs, SimStatus* sim_status) {
	for (int i = 0; i < nr_units; i++)
	{
		if (Q_units_array[i].busy && (Q_units_array[i].exec_cycles_left == 0)) { //unit is busy but done calculation
			if (unit_write_back(D_units_table, Q_units_table, &Q_units_array[i], regs))
			{
				sim_status->insttrace[D_units_array[i].pc_inst].cycle_write_result = sim_status->clock_cycle;
				ResetUnit(&D_units_array[i]);
			}
		}
	}
}

// perform write back on all the units types
void WriteBack(UnitsTable* D_units_table, UnitsTable* Q_units_table, Reg* regs, SimStatus* sim_status) {
	UnitTypeWriteBack(Q_units_table->add_nr_units, Q_units_table->add_units_array, D_units_table->add_units_array, D_units_table, Q_units_table, regs, sim_status);
	UnitTypeWriteBack(Q_units_table->sub_nr_units, Q_units_table->sub_units_array, D_units_table->sub_units_array, D_units_table, Q_units_table, regs, sim_status);
	UnitTypeWriteBack(Q_units_table->mul_nr_units, Q_units_table->mul_units_array, D_units_table->mul_units_array, D_units_table, Q_units_table, regs, sim_status);
	UnitTypeWriteBack(Q_units_table->div_nr_units, Q_units_table->div_units_array, D_units_table->div_units_array, D_units_table, Q_units_table, regs, sim_status);
	UnitTypeWriteBack(Q_units_table->ld_nr_units,  Q_units_table->ld_units_array,  D_units_table->ld_units_array,  D_units_table, Q_units_table, regs, sim_status);
	UnitTypeWriteBack(Q_units_table->st_nr_units,  Q_units_table->st_units_array,  D_units_table->st_units_array,  D_units_table, Q_units_table, regs, sim_status);
}

// unit write back - waits to all other units to be ready to handle WAR hazard, and then perform write back on each unit
bool unit_write_back(UnitsTable* D_units_table, UnitsTable* Q_units_table, Unit* me, Reg* regs)
{
	bool unit_wb_done = 1;
	//wait for all other units to be ready to handle WAR hazard
	unit_wb_done = unit_wb_done && wait_unit_type_ready_for_wb(Q_units_table->add_nr_units, Q_units_table->add_units_array, me);
	unit_wb_done = unit_wb_done && wait_unit_type_ready_for_wb(Q_units_table->sub_nr_units, Q_units_table->sub_units_array, me);
	unit_wb_done = unit_wb_done && wait_unit_type_ready_for_wb(Q_units_table->mul_nr_units, Q_units_table->mul_units_array, me);
	unit_wb_done = unit_wb_done && wait_unit_type_ready_for_wb(Q_units_table->div_nr_units, Q_units_table->div_units_array, me);
	unit_wb_done = unit_wb_done && wait_unit_type_ready_for_wb(Q_units_table->ld_nr_units, Q_units_table->ld_units_array, me);
	unit_wb_done = unit_wb_done && wait_unit_type_ready_for_wb(Q_units_table->st_nr_units, Q_units_table->st_units_array, me);
	if (!unit_wb_done) return 0;
	// perform write back on each unit (updates relevant Ri,Rj on other units)
	perform_wb_on_unit_type(Q_units_table->add_nr_units, Q_units_table->add_units_array, D_units_table->add_units_array, me, regs);
	perform_wb_on_unit_type(Q_units_table->sub_nr_units, Q_units_table->sub_units_array, D_units_table->sub_units_array, me, regs);
	perform_wb_on_unit_type(Q_units_table->mul_nr_units, Q_units_table->mul_units_array, D_units_table->mul_units_array, me, regs);
	perform_wb_on_unit_type(Q_units_table->div_nr_units, Q_units_table->div_units_array, D_units_table->div_units_array, me, regs);
	perform_wb_on_unit_type(Q_units_table->ld_nr_units, Q_units_table->ld_units_array, D_units_table->ld_units_array, me, regs);
	perform_wb_on_unit_type(Q_units_table->st_nr_units, Q_units_table->st_units_array, D_units_table->st_units_array, me, regs);
	//update reg values - once per "me"
	strcpy(regs[GetRegNum(regs, me->Fi)].tag, "--");
	if (me->unit_type != ST) //all ops except st have res
	{
		regs[GetRegNum(regs, me->Fi)].val = me->res;
	}
	return 1; //done with wb for myself
}

void unit_read_ops_and_exec(Unit* D_unit, Unit* Q_unit, Reg* regs, Mem* mem, Unit* ST_units_array, Unit* LD_units_array, int nr_st_units, int nr_ld_units, SimStatus* sim_status) {
	bool is_busy = Q_unit->busy;
	bool rk_ready = (Q_unit->unit_type == LD) ? true : !strcmp("Yes", Q_unit->Rk); //ld op doesn't use Fk
	bool rj_ready = (Q_unit->unit_type == ST || Q_unit->unit_type == LD) ? true : !strcmp("Yes", Q_unit->Rj); //ld, st ops don't use Fj
	int cycles_left = Q_unit->exec_cycles_left;
	bool exec_started = Q_unit->exec_started;
	if (!is_busy) return; //unit is not active

	else if ((exec_started == false) && rk_ready && rj_ready) { //read operands 
		if (Q_unit->unit_type == ST)
		{
			exec_started = check_ready_for_store(Q_unit, ST_units_array, LD_units_array, nr_st_units, nr_ld_units); //Parallel Write command to same address Hazard
		}
		else if (Q_unit->unit_type == LD)
		{
			exec_started = check_ready_for_load(Q_unit, ST_units_array, nr_st_units); //check if load command is parallel to write command on same address 
		}
		else
		{
			exec_started = true;
			
		}
		if (exec_started)
		{
			strcpy(D_unit->Rk, "No");
			strcpy(D_unit->Rj, "No");
			if (D_unit->pc_inst != -1) {
				if (sim_status->insttrace[D_unit->pc_inst].cycle_read_operands == -1)
					sim_status->insttrace[D_unit->pc_inst].cycle_read_operands = sim_status->clock_cycle; // cylce read operands
			}
		}
	}

	if (exec_started == true)
	{
		
		cycles_left--; //the delay of a unit defines the cycles left to execution
		float data0 = GetRegData(regs, Q_unit->Fj);
		float data1 = GetRegData(regs, Q_unit->Fk);
		int imm = Q_unit->imm;

		if (cycles_left == 0)
		{
			sim_status->insttrace[D_unit->pc_inst].cycle_execute_end = sim_status->clock_cycle; // cylce execute ends
			exec_started = false;
			switch (Q_unit->unit_type)
			{
			case ADD:
				D_unit->res = exec_add(data0, data1);
				break;
			case SUB:
				D_unit->res = exec_sub(data0, data1);
				break;
			case MUL:
				D_unit->res = exec_mul(data0, data1);
				break;
			case DIV:
				D_unit->res = exec_div(data0, data1);
				break;
			case LD:
				D_unit->res = exec_ld(imm, mem);
				break;
			case ST:
				exec_st(data1, imm, mem);
				break;
			default:
				break;
			}									
		}
	}
	D_unit->exec_cycles_left = cycles_left;
	D_unit->exec_started = exec_started;
}

void traceunit(Unit* chosen_unit, SimStatus *sim_status, FILE* traceunit_file) {
	fprintf(traceunit_file, "%d ", sim_status->clock_cycle);
	fprintf(traceunit_file, "%s ", chosen_unit->name);
	fprintf(traceunit_file, "%s ", chosen_unit->Fi);
	fprintf(traceunit_file, "%s ", chosen_unit->Fj);
	fprintf(traceunit_file, "%s ", chosen_unit->Fk);
	fprintf(traceunit_file, "%s ", chosen_unit->Qj);
	fprintf(traceunit_file, "%s ", chosen_unit->Qk);
	fprintf(traceunit_file, "%s ", chosen_unit->Rj);
	fprintf(traceunit_file, "%s\n", chosen_unit->Rk);
}

bool is_chosen_unit(Unit* unit, char* chosen_unit) {

	if (!strcmp(unit->name, chosen_unit)) {
		return true;
	}

	return false;
}

void print_instrace(char* path, SimStatus* sim_status) {
	FILE* instrace_file = fopen(path, "w");
	char hex_string[9];
	for (int i = 0; i < MEM_LEN; i++) {
		if (sim_status->insttrace[i].pc == -1) break;
		dec_to_n_chars_hex(hex_string, sim_status->insttrace[i].inst, 8);
		fprintf(instrace_file, "%s ", hex_string);
		fprintf(instrace_file, "%d ", sim_status->insttrace[i].pc);
		fprintf(instrace_file, "%s ", sim_status->insttrace[i].unit);
		fprintf(instrace_file, "%d ", sim_status->insttrace[i].cycle_issued);
		fprintf(instrace_file, "%d ", sim_status->insttrace[i].cycle_read_operands);
		fprintf(instrace_file, "%d ", sim_status->insttrace[i].cycle_execute_end);
		fprintf(instrace_file, "%d\n", sim_status->insttrace[i].cycle_write_result);
	}
	fclose(instrace_file);
}