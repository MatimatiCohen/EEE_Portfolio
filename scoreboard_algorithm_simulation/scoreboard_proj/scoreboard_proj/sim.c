#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "utils.h"
#include "FuncUnit.h"
#include "InstQueue.h"
#include "Routine.h"
#include "MemResources.h"

int main(int argc, char* argv[])
{
	//Create Instances
	ConfField conf_fields[NUM_CONF_FIELDS];
	Mem mem;
	UserConf user_conf;
	Reg regs[NUM_REGS];
	UnitsTable D_units_table,Q_units_table; // D = Next State  ; Q = Current State
	InstQueue inst_queue;
	SimStatus sim_status;
	FILE* traceunit_file = fopen(argv[TRACEUNIT_FILE_ARG], "w");
	if (traceunit_file == NULL) return 0;	

	//initialize resources (memory, registers, queue,units, simulator status)
	InitQueue(&inst_queue);
	InitRegs(regs);
	GetArgs(conf_fields, &user_conf , &mem, argc, argv);
	CreateUnitsTable(&D_units_table, &user_conf);
	CreateUnitsTable(&Q_units_table, &user_conf);
	InitSimStatus(&sim_status);			

	while (!sim_status.halt || !sim_status.all_units_done)
	{
		//fetch to queue
		if (!isFull(&inst_queue) && !sim_status.halt && !sim_status.last_inst) {
			Fetch(&inst_queue, &mem, &sim_status);
		}
		//issue ready instructions
		// note: issue need to begin on cycle 1 - a cycle after the first fetch.
		if (!isEmpty(inst_queue) && sim_status.clock_cycle) {
			Issue(&inst_queue, &D_units_table, &Q_units_table, regs, &sim_status);
		}

		//execute ready instructions
		ReadOpsAndExec(&D_units_table, &Q_units_table, regs, &mem, &sim_status, traceunit_file);

		//write result for ready instructions
		WriteBack(&D_units_table, &Q_units_table, regs, &sim_status);

		//switch all FFs to next state
		unit_NS_update(&D_units_table, &Q_units_table);
		sim_status.clock_cycle++;
		sim_status.all_units_done = CheckAllUnitsDone(&Q_units_table);
		if ((!sim_status.last_inst) && (!isFull(&inst_queue))) {
			sim_status.pc++;
		}
	}
	
	//print logs & close files
	PrintRegs(regs, argv);
	PrintMem(&mem, argv);
	print_instrace(argv[TRACEINST_FILE_ARG], & sim_status);
	fclose(traceunit_file);

	//free allocated memmory
	FreeUnitsTable(&D_units_table);
	FreeUnitsTable(&Q_units_table);

	exit(0);
}