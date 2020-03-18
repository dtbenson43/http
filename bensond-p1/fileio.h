#ifndef FILEIO_H
#define FILEIO_H

#define FILE_BUFFER_SIZE 1001
#define FILE_LINE_BUFFER 30000

int setFile(char *filepath);

int getLine(char *destString);

int getText(char *destString);

int getFileLength();

int closeFile();

#endif