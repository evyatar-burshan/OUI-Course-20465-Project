/******************************************************
 * Function: export_files
 * Description: Exports data and code segments, as well as external and entry symbols, to output files.
 * 
 * @param headDataImg: Pointer to the head of the data image linked list.
 * @param headCodeImg: Pointer to the head of the code image linked list.
 * @param headExtEnt: Pointer to the head of the external/entry symbol linked list.
 * @param fileName: Name of the output file.
 * @param IC: Instruction counter.
 * @param DC: Data counter.
 ******************************************************/
void export_files(ptrDataImg *headDataImg, ptrCodeImg *headCodeImg, ptrExtEnt *headExtEnt, char* fileName, int IC, int DC);
