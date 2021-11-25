//
// Keyboard console event handling

void keyEventEnter(void);
void keyEventBackspace(void);
void keyEventAcceptChar(uint8_t);

void updateKeyboard(void) {
	
	uint8_t scanCodeAccepted = 0;
	char currentChar=0x00;
	
	uint8_t readKeyCode;
	
	// Read keyboard character
	const char deviceName[] = "keyboard";
	EntryPtr keyboardPtr = getFuncAddress(deviceName, sizeof(deviceName));
	callExtern(keyboardPtr, 0x00, readKeyCode);
	
	// Check MAX string length
	if (console.keyboard_string_length < 19) {
		// ASCII char
		currentChar = readKeyCode;
		if (readKeyCode > 0x1f) {scanCodeAccepted=1;}
	} else {
		// Allow special keys past max string length
		if (readKeyCode < 0x20) currentChar = readKeyCode;
	}
	
	// Prevent wild key repeats
	if (console.lastChar == currentChar) {console.lastChar == currentChar; return;} console.lastChar = currentChar;
	
	// Check special keys
	switch (currentChar) {
		
		case 0x01: keyEventBackspace(); break;
		case 0x02: keyEventEnter(); break;
		
		default: break;
	}
	
	// Check key accepted
	if (scanCodeAccepted == 1) 
		keyEventAcceptChar(currentChar);
	
	return;
}

void keyEventEnter(void) {
	
	uint8_t currentKeyStringLength = console.keyboard_string_length;
	console.keyboard_string_length = 0;
	
	if (console.cursorLine == 3) {console.shiftUp();}
	if (console.cursorLine == 2) console.cursorLine++;
	if (console.cursorLine == 1) console.cursorLine++;
	if (console.cursorLine == 0) {if (console.promptState == 0) {console.promptState++;} else {console.cursorLine++;}}
	
	// Execute the function
	if (currentKeyStringLength > 0) {console.cursorPos=0;
		
		// Function look up
		for (uint8_t i=0; i<_COMMAND_TABLE_SIZE__; i++) {
			
			if (moduleTable.functionNameIndex[i][0] == 0x20) continue;
			
			// Extract function name
			char functionName[_COMMAND_TABLE_NAME_SIZE__];
			for (uint8_t a=0; a < _COMMAND_TABLE_NAME_SIZE__; a++) functionName[a] = moduleTable.functionNameIndex[i][a];
			
			if (string_compare(functionName, console.keyboard_string, currentKeyStringLength) == 0) continue;
			
			// Update memory access range
			kernel.setMemoryAccess();
			
			// Execute the command
			moduleTable.command_function_table[i]();
			
			break;
		}
		
	}
	
	console.clearString();
	console.printPrompt();
	return;
}

void keyEventBackspace(void) {
	if (console.keyboard_string_length > 0) {
		
		// Remove last char from display
		console.setCursorPosition(console.cursorLine, console.keyboard_string_length);
		console.printSpace();
		console.setCursorPosition(console.cursorLine, console.keyboard_string_length);
		
		// and keyboard string
		console.keyboard_string_length--;
		console.keyboard_string[console.keyboard_string_length] = 0x20;
	}
	return;
}

void keyEventAcceptChar(uint8_t new_char) {
	
	// Add the ASCII char
	console.keyboard_string[console.keyboard_string_length] = new_char;
	console.keyboard_string_length++;
	
	console.printChar((char&)new_char);
	console.setCursorPosition(console.cursorLine, console.cursorPos);
	
	return;
}
