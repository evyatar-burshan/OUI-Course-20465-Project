/******************************************************
 * Function: is_number
 * Description: Checks if a string represents a number.
 * 
 * @param input: The input string to check.
 * @return YES if the string represents a number, NO otherwise.
 ******************************************************/
int is_number(char* input);
/******************************************************
 * Function: is_label
 * Description: Checks if a string represents a label.
 * 
 * @param line: The input string to check.
 * @param h: Pointer to the symbol table.
 * @return Pointer to the label node if found, NULL otherwise.
 ******************************************************/
ptrLabel is_label(char* line, ptrLabel h);
/******************************************************
 * Function: is_ext_ent
 * Description: Checks if a string represents an external entry.
 * 
 * @param line: The input string to check.
 * @param h: Pointer to the external entry table.
 * @return Pointer to the external entry node if found, NULL otherwise.
 ******************************************************/
ptrExtEnt is_ext_ent(char* line, ptrExtEnt h);
/******************************************************
 * Function: is_valid_word
 * Description: Checks if a string represents a valid word.
 * 
 * @param word: The input string to check.
 * @return YES if the string represents a valid word, NO otherwise.
 ******************************************************/
int is_valid_word(char* word);
