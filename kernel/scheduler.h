//
// Co-operative scheduler

#ifndef ____TASK_SCHEDULER__
#define ____TASK_SCHEDULER__

#include <util/delay.h>

#include <avr/interrupt.h>

#define _PROCESS_LIST_SIZE__  30
#define _PROCESS_NAME_SIZE__  10

#define _TASK_USER__          'u' // Task is a user program
#define _TASK_SERVICE__       's' // Task is a system service routine
#define _TASK_VOLATILE__      'v' // Task will run once and terminate

#ifdef __CORE_SCHEDULER_

typedef void(*Process)();


// Schedule a new process
uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type);
// Get a process index by its process name
uint8_t getProcessIndex(const char process_name[], uint8_t name_length);
// Stop a running process
uint8_t killProcess(const char process_name[], uint8_t name_length);

void __scheduler_init_(void);
void __scheduler_start(uint8_t timer_priority);
void __scheduler_stop(void);


//
// Process information descriptor table

struct ProcessDescriptorTable {
	
	uint8_t  processName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_SIZE__];
	uint8_t  processType[_PROCESS_LIST_SIZE__];
	uint16_t processPriority[_PROCESS_LIST_SIZE__];
	uint16_t processCounters[_PROCESS_LIST_SIZE__];
	void (*process_pointer_table[_PROCESS_LIST_SIZE__])();
	
}static proc_info;


//
// Schedule a new process

uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type) {
	
	if ((priority == 0) || (name_length > _PROCESS_NAME_SIZE__-1)) return 0;
	
	uint8_t index;
	
	// Find an available slot
	for (index=0; index <= _PROCESS_LIST_SIZE__; index++)
	if (proc_info.processPriority[index] == 0) break;
	
	// No free slots
	if (index == _PROCESS_LIST_SIZE__) return 0;
	
	// Launch the new process
	for (uint8_t a=0; a < name_length-1; a++)
	proc_info.processName[index][a] = name[a];
	
	proc_info.processType[index]           = task_type;
	proc_info.processPriority[index]       = priority;
	proc_info.processCounters[index]       = 0;
	proc_info.process_pointer_table[index] = task_ptr;
	
	return 1;
}

//
// Get a process index by its process name

uint8_t getProcessIndex(const char process_name[], uint8_t name_length) {
	
	if (name_length > _PROCESS_NAME_SIZE__-1) return 0;
	
	for (uint8_t index=0; index< _PROCESS_LIST_SIZE__; index++) {
		
		if (proc_info.processName[index][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			char nameChar = proc_info.processName[index][a];
			if (nameChar == process_name[a]) count++; else break;
		}
		
		return (index + 1);
	}
	
	return 0;
}

//
// Stop a running process

uint8_t killProcess(const char process_name[], uint8_t name_length) {
	
	uint8_t PID = getProcessIndex(process_name, name_length);
	
	proc_info.processType[PID]     = 0;
	proc_info.processPriority[PID] = 0;
	proc_info.processCounters[PID] = 0;
	proc_info.process_pointer_table[PID] = 0;
	
	for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++)
	proc_info.processName[PID][i] = 0x20;
	
	return 1;
}


//
// Interrupt timer 0

ISR (TIMER0_OVF_vect) {
	
	for (uint8_t PID=0; PID < _PROCESS_LIST_SIZE__; PID++) {
		
		if (proc_info.processPriority[PID] == 0) continue;
		
		if (proc_info.processCounters[PID] >= proc_info.processPriority[PID]) {
			proc_info.processCounters[PID]=0;
			proc_info.process_pointer_table[PID]();
		} else {
			proc_info.processCounters[PID]++;
			continue;
		}
		
		
		// Switch process type
		switch (proc_info.processType[PID]) {
			
			// Terminate volatile tasks
			case _TASK_VOLATILE__: {
				for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++) proc_info.processName[PID][i] = 0x20;
				proc_info.processType[PID] = 0;
				proc_info.processPriority[PID] = 0;
				proc_info.processCounters[PID] = 0;
				proc_info.process_pointer_table[PID] = 0;
				break;
			}
			
			
			default: break;
			
		}
		
		
	}
	
	return;
}

void __scheduler_init_(void) {
	for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
		proc_info.processType[i]     = 0x00;
		proc_info.processPriority[i] = 0x00;
		proc_info.processCounters[i] = 0x00;
		proc_info.process_pointer_table[i] = 0;
		
		for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++)
			proc_info.processName[i][a] = 0x20;
	}
}

void __scheduler_start(uint8_t timer_priority) {
	
	_delay_ms(100);
	
	TCCR0A  = 0x00;          // Normal counter and waveform
	TCCR0B |= (1 << CS10);   // Timer mode with NO prescaler
	TIMSK0 |= (1 << TOIE0);  // Enable timer overflow interrupt
	
	TCNT0 = timer_priority;  // Set timer priority rate
	sei();                   // Enable interrupts
}

void __scheduler_stop(void) {
	
	_delay_ms(100);
	
	TCCR0A  = 0x00;
	TCCR0B &= ~(1 << CS10);
	TIMSK0 &= ~(1 << TOIE0);
	
	TCNT0 = 0;
}


#endif

#endif
