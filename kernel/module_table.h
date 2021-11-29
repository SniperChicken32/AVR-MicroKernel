//
// Command line function table

struct CommandFunctionTable {
	
	char functionNameIndex[_COMMAND_TABLE_SIZE__][_COMMAND_TABLE_NAME_SIZE__];
	void (*command_function_table[_COMMAND_TABLE_SIZE__])(void);
	
	CommandFunctionTable() {
		for (uint8_t i=0; i < _COMMAND_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a < _COMMAND_TABLE_NAME_SIZE__; a++) functionNameIndex[i][a] = 0x20;
		}
	}
	
};
CommandFunctionTable moduleTable;

// Install a function pointer into the function table
uint8_t loadModule(void(*function_ptr)(), const char name[], uint8_t name_length) {
	
	if (name_length > _COMMAND_TABLE_NAME_SIZE__) return 0;
	
	// Find a free slot
	uint8_t index;
	for (index=0; index < _COMMAND_TABLE_SIZE__; index++) 
		if (moduleTable.functionNameIndex[index][0] == 0x20) break;
	
	// Set the name and pointer
	for (uint8_t i=0; i < name_length-1; i++)
		moduleTable.functionNameIndex[index][i] = name[i];
	
	moduleTable.command_function_table[index] = function_ptr;
	
	return 1;
}

