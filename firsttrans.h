/******************************************************
 * Function: translate_line
 * Description: Translates a single line of code from the assembly file.
 * 
 * @param curLine: Current line of code.
 * @param headLabel: Pointer to the head of the label linked list.
 * @param headDataImg: Pointer to the head of the data image linked list.
 * @param headCodeImg: Pointer to the head of the code image linked list.
 * @param fileName: Name of the assembly file.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
void firsttrans(FILE* am, char* fileName);
