#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fileio.h"

FILE *_file = NULL;
char _fpath[FILE_BUFFER_SIZE] = "\0";
int _cursor = 0;
int _fileLength = -1;

int setFile(char *filepath) {
    closeFile();
    strcpy(_fpath, filepath);
    _file = fopen(_fpath, "r");
    _fileLength = -1;
    _cursor = 0;
    if (_file == NULL)
    {
      return 0;
    }
    else return 1;
}

int closeFile() {
    if (_file != NULL) {
        fclose(_file);
        _file = NULL;
        return 1;
    } else {
        return 0;
    }
}

int getLine(char *destString) {
    if (feof(_file)) {
        _cursor = 0;
        fseek(_file, _cursor, SEEK_SET);
        return 0;
    }
    char buffer[FILE_LINE_BUFFER] = "\0";
    int bufferIdx = 0;
    
    fseek(_file, _cursor, SEEK_SET);
    char currentChar = fgetc(_file);
    while(currentChar != '\n' && !feof(_file)) {
        buffer[bufferIdx++] = currentChar;
        currentChar = fgetc(_file);
    }
    
    buffer[bufferIdx] = '\0';
    
    _cursor += bufferIdx + 1;
    strcpy(destString, buffer);
    
    return _cursor;
}

int getFileLength() {
    if (_fileLength == -1) {
        _cursor = 0;
        int length = 0;
        char tempString[FILE_BUFFER_SIZE];
        while(getLine(tempString)) {
            length = _cursor;
        }
        _fileLength = length;
    }
    return _fileLength - 1;
}

int getText(char *destString) {
    _cursor = 0;
    int length = getFileLength();
    char tempLine[FILE_BUFFER_SIZE];
    char tempString[length];
    tempString[0] = '\0';
    while(getLine(tempLine)) {
        strcat(tempString, tempLine);
        if (strlen(tempString) != length - 1)
            strcat(tempString, "\n");
    }
    tempString[length] = '\0';
    strcpy(destString, tempString);
    return 1;
}