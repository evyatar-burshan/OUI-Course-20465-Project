/******************************************************
 * Function: decimalToBinary
 * Description: Converts a decimal number to a binary string.
 * 
 * @param decNum: The decimal number to convert.
 * @return The binary string.
 ******************************************************/
char* decimalToBinary(int decNum);
/******************************************************
 * Function: decimalToBinaryARE
 * Description: Converts a decimal number to a binary string
 *              and appends ARE bits based on the ARE parameter.
 * 
 * @param decNum: The decimal number to convert.
 * @param ARE: The ARE bits to append (A, R, or E).
 * @return The binary string.
 ******************************************************/
char* decimalToBinaryARE(int decNum, int ARE);
/******************************************************
 * Function: decimalToBinaryARE
 * Description: Converts a decimal number to a binary string
 *              and appends ARE bits based on the ARE parameter.
 * 
 * @param decNum: The decimal number to convert.
 * @param ARE: The ARE bits to append (A, R, or E).
 * @return The binary string.
 ******************************************************/
char* registerToBinary (int sourceRegisterNum, int destinationRegisterNum);
/******************************************************
 * Function: operationToBinary
 * Description: Converts an operation code and its addressing modes
 *              to a binary string, appending ARE bits based on
 *              the ARE parameter.
 * 
 * @param opcode: The operation code (0-15).
 * @param addressingModeSource: The addressing mode for the source operand (0-3).
 * @param addressingModeDestination: The addressing mode for the destination operand (0-3).
 * @param ARE: The ARE bits to append (A, R, or E).
 * @return The binary string.
 ******************************************************/
char* operationToBinary (int opcode, int addressingModeSource, int addressingModeDestination, int ARE);
