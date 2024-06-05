/******************************************************
 * Function: secondtrans
 * Description: Performs the second pass of the assembly process, generating the output files.
 * 
 * @param am: Pointer to the modified assembly file.
 * @param headLabel: Pointer to the pointer to the head of the label linked list.
 * @param headDataImg: Pointer to the pointer to the head of the data image linked list.
 * @param headCodeImg: Pointer to the pointer to the head of the code image linked list.
 * @param fileName: Name of the output files.
 * @param IC: Value of the Instruction Counter.
 * @param DC: Value of the Data Counter.
 ******************************************************/
void secondtrans(FILE* am, ptrLabel *headLabel, ptrDataImg *headDataImg, ptrCodeImg *headCodeImg, char* fileName, int IC, int DC);
