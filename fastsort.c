// P1a.c : Defines the entry point for the console application.
//

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int wordNumber = 0;						//we use a global variable for the word number parameter
											//so we can use it in our string comparison function
const int maxLineLength = 129;			//one extra character for a line end

//comparison funtion
int my_compare(const void *a, const void *b) {

	//we need to convert our comparison function inputs to char pointers
	const char * iA = * (const char **) a;
	const char * iB = * (const char **) b;

	//and we need to allocate some memory for variables to copy the strings to
		//this is so that we can tokenize iA and iB without changing them
	char * iA2token = (char *) malloc(strlen(iA) + 1);
	char * iB2token = (char *) malloc(strlen(iB) + 1);
	strcpy(iA2token, iA);
	strcpy(iB2token, iB);

	//now we tokenize string A and create a string (strA) where we keep a substring
		//of it - we start by just the first word
	char * tokenA = strtok((char *) iA2token, " ");
	char * strA[129];
	strA[0] = (char *) malloc(strlen(tokenA) + 1);
	strcpy(strA[0], tokenA);
	char * wordToCompareA = NULL;
	int i = 1;

	//and then we add a number of words so that the number of words in our substring
		//is equal to the numeric parameter passed in
	for (i = 1; (i <= wordNumber) && (tokenA != NULL); i++) {
		//we set tokenA to the next word
		tokenA = strtok(NULL, " ");
		//if we're not at the end of the current line yet, we add a word to strA
			//and continue looping, looking for the word to compare
		if (tokenA != NULL) {
			strA[i] = (char *) malloc(strlen(tokenA) + 1);
			strA[i] = strdup(tokenA);
		}
		//if we're at the end of the current line, we store the last word on the
			//current line and use that as the word to compare
		else {
			wordToCompareA = strdup(strA[i - 1]);
			break;
		}
	}

	//if we still don't have a word to compare, we use the last token we were on
	if (wordToCompareA == NULL) {
		wordToCompareA = strdup(tokenA);
	}

	//and then we do the same thing for the string we're comparing with
	char * tokenB = strtok((char *) iB2token, " ");
	char * strB[129];
	char * wordToCompareB = NULL;

	strB[0] = (char *)malloc(strlen(tokenB) + 1);
	strcpy(strB[0], tokenB);
	int j = 1;

	for (j = 1; (j <= wordNumber) && (tokenB != NULL); j++) {
		tokenB = strtok(NULL, " ");
		if (tokenB != NULL) {
			strB[j] = (char *)malloc(strlen(tokenB) + 1);
			strB[j] = strdup(tokenB);
		}
		else {
			wordToCompareB = strdup(strB[j - 1]);
			break;
		}
	}

	if (wordToCompareB == NULL) {
		wordToCompareB = strdup(tokenB);
	}

	return strcmp(wordToCompareA, wordToCompareB);
}

void removeNonNumericChars(char * strInput, char charToRemove)
{
	char *stringToScan = strInput, *stringToChange = strInput;
	while (*stringToScan) {
		*stringToChange = *stringToScan++;
		if (*stringToChange != charToRemove)
		{
			stringToChange = stringToChange + 1;
		}
		//pw += (*pw != charToRemove);
	}
	*stringToChange = '\0';
}

int main(int argc, char *argv[])
{
	//we first process inputs - setting wordNumber and the input file
	char *input_file = NULL;

	if (argc == 2) {
		//printf("file: %s\n", argv[1]);
		input_file = argv[1];
	}
	else if (argc == 3) {
		//skip the second arg
		//printf("file: %s\n", argv[2]);
		//removeNonNumericChars(argv[1], '[');
		//removeNonNumericChars(argv[1], ']');
		if (atoi(argv[1]) < 0) {
			wordNumber = -atoi(argv[1]);
		}
		wordNumber = wordNumber - 1;
		if (wordNumber == -1) {
			wordNumber = 0;
		}
		else 
		input_file = argv[2];
	}
	else {
		fprintf(stderr, "Error: Bad command line parameters\n");
		exit(1);
	}

	if (input_file == NULL) {
	  fprintf(stderr, "Error: Bad command line parameters\n");
	  exit(1);
	}

	FILE *fp = fopen(input_file, "r");
	if (fp == NULL) {
	  fprintf(stderr, "Error: Cannot open file %s\n", input_file);
	  //fprintf(stderr, "Error: Bad command line parameters\n");
		exit(1);
	}

	int lineAllocation = 100;											//we allocate 100 lines to start - we reallocate later if we need more

	char **textLines = (char **) malloc(sizeof(char*) * lineAllocation);		//allocating a pointer to pointers of char - will be used as
																					//an array of text with one entry per line of text
	if (textLines == NULL)
	{
		fprintf(stderr, "malloc failed.\n");
		exit(1);
	}

	int line;
	for (line = 0; 1; line++)													//rather than get the number of lines in the file in one pass and increment
	{																				//line that many times, we just keep incrementing it until fgets gives
	  int lineLength = 0;																//us NULL - then we know we're done

		if (line >= lineAllocation)
		{
			int biggerLineAllocation = lineAllocation * 2;							//if it turns out that we didn't allocate enough lines,
																						//double our allocation and try again
			textLines = (char **) realloc(textLines, sizeof(char*) * biggerLineAllocation);

			if (textLines == NULL)
			{
				fprintf(stderr, "malloc failed.\n");
				exit(1);
			}

			lineAllocation = biggerLineAllocation;
		}

		textLines[line] = (char *) malloc(maxLineLength);							//allocate memory for the current line

		if (textLines[line] == NULL)
		{
			fprintf(stderr, "malloc failed.\n");
			exit(1);
		}

		if (fgets(textLines[line], 2147483647, fp) == NULL)					//break out of the loop if we're at the end of the file
		{
			free(textLines[line]);
			textLines[line] = NULL;
			break;
		}

		if (strlen(textLines[line]) > 128) {
		  fprintf(stderr, "Line too long\n"); 
		  exit(1);
		}

		//strip line feeds and carriage returns from the end of lines and replace them with null chars so the tokenizer will work
		for (lineLength = strlen(textLines[line]) - 1; lineLength >= 0 && ((textLines[line][lineLength] == '\n') || (textLines[line][lineLength] == '\r')); lineLength--)
		{
			textLines[line][lineLength] = '\0';
		}
	}

	fclose(fp);								//We've got what we need in an array now, so we close the file

	int lines = line - 1;					//we broke earlier because the line number was 1 too high
	//int lineLength = strlen(textLines[lines]) - 1;
	line = 0;

	//printf("%s\n", "Original File:\n");
	//for (line = 0; line <= lines; line++)
	//{
	//	printf("%s\n", textLines[line]);
	//}

	qsort(textLines, lines + 1, sizeof(textLines), my_compare);

	line = 0;

	//printf("\n%s\n", "\n----------\n");
	//printf("%s%d%s\n", "File sorted on word #", (wordNumber + 1),":\n");
	for (line = 0;
 line <= lines; line++)
	{
		printf("%s\n", textLines[line]);
	}

	line = 0;

	//we've printed the sorted array - now we can free the memory
	for (line = 0; line <= lines; line++)
	{
		free(textLines[line]);
		textLines[line] = NULL;
	}
	free(textLines);

    return 0;
}
