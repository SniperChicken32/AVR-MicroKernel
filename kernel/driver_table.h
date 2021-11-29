//
// Device driver entry pointer table

// Load a device driver entry point function onto the driver function table
uint8_t   loadLibrary(const char name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));

// Get a library function address by its device name
EntryPtr& getFuncAddress(const char device_name[], uint8_t name_length);

// Call an external function from a library function pointer
uint8_t   callExtern(EntryPtr& library_function, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL);


struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_TABLE_NAME_SIZE__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++)
		for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++)
		deviceNameIndex[i][a] = 0x20;
		
	}
	
};
DeviceDriverTable deviceDriverTable;

uint8_t loadLibrary(const char name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__-1) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length-1; i++)
	deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = driver_ptr;
	
	return 1;
}

EntryPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return (EntryPtr&)NULL_f;
	
	// Function look up
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
		}
		
		if (count >= name_length) return deviceDriverTable.driver_entrypoint_table[i];
		
	}
	
	return (EntryPtr&)NULL_f;
}

uint8_t callExtern(EntryPtr& library_function, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	// Check valid pointer
	if (library_function == (EntryPtr&)NULL_f) return 0;
	
	library_function(function_call, paramA, paramB, paramC, paramD);
	
	return 1;
}

