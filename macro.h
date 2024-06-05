/******************************************************
 * Function: pre_processor
 * Description: Preprocesses the assembly file, handling macros and generating a modified assembly file.
 * 
 * @param as: Pointer to the original assembly file.
 * @param fileNameAm: Name of the macro file to be generated.
 * @param fileNameAs: Name of the modified assembly file to be generated.
 * @return: int indicating whether an error occurred (YES) or not (NO).
 ******************************************************/
int pre_processor (FILE *as, char* fileNameAm, char* fileNameAs);
