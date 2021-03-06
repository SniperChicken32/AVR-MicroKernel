//
// Audio test app

void __application_main_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "beep"

struct ModuleLoaderBeep {
	ModuleLoaderBeep() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)__application_main_, _DEVICE_TYPE_MODULE__);
	}
}static beepModuleLoader;
#undef __MODULE_NAME_

//
// Entry point

void __application_main_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus device_bus;
	device_bus.waitstate_write = 0;
	
	uint32_t freq  = console.keyboard_string[5];
	uint8_t length = 100;
	uint8_t delta  = 0;
	
	if ((freq >= '0') && (freq <= '9')) {delta = freq - '0';}
	if ((freq >= 'a') && (freq <= 'z')) {delta = freq - 'a';}
	
	Device audioDriver;
	if (get_func_address(_INTERNAL_SPEAKER__, sizeof(_INTERNAL_SPEAKER__), audioDriver) == 0) return;
	call_extern(audioDriver, 0x00, delta, length);
	
	return;
}


