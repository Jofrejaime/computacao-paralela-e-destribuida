/*
// Command line reader (implementation), version 1
*/

#include <stdio.h>
#include <string.h>

int readLineArguments(char **argVector, int vectorSize, char *buffer, int buffersize) {
  int numtokens = 0;
  const char *separators = " \r\n\t";
  int i;
  char *token;

  if (argVector == NULL || buffer == NULL || vectorSize <= 0 || buffersize <= 0) {
    return 0;
  }

  if (fgets(buffer, buffersize, stdin) == NULL) {
    return -1;
  }

  token = strtok(buffer, separators);
  while (numtokens < vectorSize - 1 && token != NULL) {
    argVector[numtokens] = token;
    numtokens++;
    token = strtok(NULL, separators);
  }

  for (i = numtokens; i < vectorSize; i++) {
    argVector[i] = NULL;
  }

  return numtokens;
}
