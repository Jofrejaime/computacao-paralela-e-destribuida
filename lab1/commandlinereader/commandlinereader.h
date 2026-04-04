/*
// Command line reader (header file), version 1
*/

#ifndef COMMANDLINEREADER_H
#define COMMANDLINEREADER_H

/*
Reads up to 'vectorSize' space-separated arguments from standard input
and stores them in 'argVector'.
The argument list is always NULL-terminated.

Return value:
 Number of arguments read, or -1 on EOF/error.
*/
int readLineArguments(char **argVector, int vectorSize, char *buffer, int buffersize);

#endif
