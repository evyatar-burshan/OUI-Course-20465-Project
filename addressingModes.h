/******************************************************
 * Function: detect_addressing_mode
 * Description: Detects the addressing mode based on the input string.
 * 
 * @param input: The input string to detect the addressing mode.
 * @return The detected addressing mode (0-3, or -1 if not detected).
 ******************************************************/
int detect_addressing_mode(char* input);
/******************************************************
 * Function: immediate_addressing
 * Description: Processes immediate addressing mode
 *		and return binary string representing the input.
 * 
 * @param input: The input string representing the immediate value or label.
 * @param h: Pointer to the symbol table.
 * @return The binary string representing the immediate value.
 ******************************************************/
char* immediate_addressing(char* input, ptrLabel h);
/******************************************************
 * Function: direct_addressing
 * Description: Processes direct addressing mode
 *		and return binary string representing the input.
 *
 * @param input: The input string representing the label.
 * @param h: Pointer to the symbol table.
 * @return The binary string representing the address.
 ******************************************************/
char* direct_addressing(char* input, ptrLabel h);
/******************************************************
 * Function: index_addressing
 * Description: Processes index addressing mode
 *		and return binary string representing the input.
 * 
 * @param input: The input string representing the label and index.
 * @param h: Pointer to the symbol table.
 * @return The binary string(s) representing the address and index.
 ******************************************************/
char* index_addressing(char* input, ptrLabel h);
/******************************************************
 * Function: direct_register_addressing
 * Description: Processes direct register addressing mode
 *		and return binary string representing the input.
 * 
 * @parame sourceRegister: The source register (1-7, 0 if NULL).
 * @param destinationRegister: The destination register (1-7, 0 if NULL).
 * @return The binary string representing the registers.
 ******************************************************/
char* direct_register_addressing(char* sourceRegister, char* destinationRegister);
