#include <stdio.h>
#include <string.h>
#include "hashMap.h"

static char* g_directories[20];
static unsigned int g_directoriesN;

void parse(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap);
void handlePreprocessing(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap);
void handleConditionResult(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap, unsigned char conditionResult);

unsigned char isAlphaNumeric(char c) {
    return (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9');
}

void getNextToken(FILE* f, char buff[80]) {
    char token[80];
    char currentCh;
    int curPos;

    currentCh = fgetc(f);
    
    if (isAlphaNumeric(currentCh)) {
        curPos = 1;
        
        token[0] = currentCh;
        
        while (isAlphaNumeric(currentCh = fgetc(f))) {
            token[curPos] = currentCh;
            curPos++;
        }

        token[curPos] = '\0';
	if(currentCh != EOF)
        	fseek (f , -1 , SEEK_CUR);
        
        strcpy(buff, token);
    }
    else {
        token[0] = currentCh;
        token[1] = '\0';
    
        strcpy(buff, token);
    }
}

void processOutput(FILE* f, char buff[80]) {
    if (f) {
        fprintf(f, "%s", buff);
    }
    else {
        printf("%s", buff);
    }
}

void lookUpNextToken(FILE* f, char buff[80]) {
    getNextToken(f, buff);
    fseek (f , -1 * strlen(buff) , SEEK_CUR);
}

void handleDefine(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];
    char key[80];
    char* value;
    value = (char*)malloc(sizeof(char) * 80);
    if (!value)
        exit(12);
    
    strcpy(value, "");
    
    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    strcpy(key, buffer);

    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    strcpy(value, buffer);

    getNextToken(inputFile, buffer);

    while(strcmp(buffer, "\n") != 0)
    {

         strcat(value,buffer);
         if(!strcmp(buffer,"\\"))
         {
             getNextToken(inputFile, buffer);
             strcat(value,buffer);

            if( 80 - strlen(value) < strlen(buffer) )
            {
                 value = (char*)realloc(value,sizeof(char)*(sizeof(value)+ 80*2));
                 if (!value)
                     exit(12);
            }
         }
        
        getNextToken(inputFile, buffer);
     
    }

    setValue(hashMap, key, value);
    free(value);
}

void handleUndefine(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];
    char key[80];
    
    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    strcpy(key, buffer);

    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    while (buffer[0] != '\n') {
        getNextToken(inputFile, buffer);
    }

    deleteValue(hashMap, key);
}


void handleInclude(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];
    char path[80];
    char newPath[80];
    int it;
    FILE* header;

    strcpy(path, "");

    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    getNextToken(inputFile, buffer);
    
    while (buffer[0] != '\"')
    {
        strcat(path, buffer);
        getNextToken(inputFile, buffer);
    }

    while (buffer[0] != '\n') {
        getNextToken(inputFile, buffer);
    }
    
    header = fopen(path, "r");
    
    if(header){
        parse(header, outputFile, hashMap);
        fclose(header);
    }
    else {
        it = 0;
        for(it = 0; it < g_directoriesN; it++) {
            strcpy(newPath, g_directories[it]);
            strcat(newPath, path);
            
            header = fopen(newPath, "r");
           
           if(header){
               parse(header, outputFile, hashMap);
               fclose(header);
               break;
           }
        }
    }
}

unsigned char evalAndConsumeCondition(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];
    char value[80];
    char* mapVal;
    unsigned char check;
    
    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    strcpy(value, buffer);

    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    while (buffer[0] != '\n') {
        getNextToken(inputFile, buffer);
    }

    mapVal = getValue(hashMap, value);
    
    if (mapVal) {
        check = strcmp(mapVal, "0");
    }
    else {
        check = strcmp(value, "0");
    }

    return check;
}

void consumeUntilEnd(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap, unsigned char status) {
    char buffer[80];

    getNextToken(inputFile, buffer);
    
    while (buffer[0] != EOF) {

        if(!strcmp(buffer, "#"))
        {
            lookUpNextToken(inputFile, buffer);
            if (!strcmp(buffer, "endif")) {
                while (buffer[0] != '\n') {
                    getNextToken(inputFile, buffer);
                }
                return;
            }
        }

        getNextToken(inputFile, buffer);
    }
}

unsigned char consumeUntilEndOrElse(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap, unsigned char status) {
    char buffer[80];
    unsigned char result;

    getNextToken(inputFile, buffer);
    
    while (buffer[0] != EOF) {

        if(!strcmp(buffer, "#"))
        {
            lookUpNextToken(inputFile, buffer);
            if (!strcmp(buffer, "endif")) {
                while (buffer[0] != '\n') {
                    getNextToken(inputFile, buffer);
                }
                return 0;
            }
            else if (!strcmp(buffer, "else")) {
                while (buffer[0] != '\n') {
                    getNextToken(inputFile, buffer);
                }
                return 1;
            }
            else if (!strcmp(buffer, "elif")) {
                if (!status) {
                    getNextToken(inputFile, buffer);
                    result = evalAndConsumeCondition(inputFile, outputFile, hashMap);
                    handleConditionResult(inputFile, outputFile, hashMap, result);
                }
                else {
                    consumeUntilEndOrElse(inputFile, outputFile, hashMap, status);
                }

                return 0;
            }
        }

        getNextToken(inputFile, buffer);
    }
    
    return 0;
}


void handleConditionResult(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap, unsigned char conditionResult) {
    char buffer[80];
    unsigned char inLiteralContext;
    char prevChr;
    char* value;
    inLiteralContext = 0;

    if (!conditionResult) {
        if (!consumeUntilEndOrElse(inputFile, outputFile, hashMap, conditionResult)) {
            return;
        }
    }

    getNextToken(inputFile, buffer);
    
    while (buffer[0] != EOF) {

        if(!strcmp(buffer, "#"))
        {
            lookUpNextToken(inputFile, buffer);
            if (!strcmp(buffer, "endif")) {
                while (buffer[0] != '\n') {
                    getNextToken(inputFile, buffer);
                }
                return;
            }
            else if (!strcmp(buffer, "else")) {
                consumeUntilEnd(inputFile, outputFile, hashMap, conditionResult);
                return;
            }
            else if (!strcmp(buffer, "elif")) {
                consumeUntilEnd(inputFile, outputFile, hashMap, conditionResult);
                return;
            }

            handlePreprocessing(inputFile, outputFile, hashMap);
        }
        else
        {
            if (inLiteralContext) {
                prevChr = '0';

                while (buffer[0] != '\"' && prevChr != '\\') {
                    processOutput(outputFile, buffer);
                    
                    prevChr = buffer[0];
                    getNextToken(inputFile, buffer);
                }
                
                processOutput(outputFile, buffer);
                
                inLiteralContext = 0;
            }
            else {
                value = getValue(hashMap, buffer);
                
                if (value) {
                    processOutput(outputFile, value);
                }
                else {
                    processOutput(outputFile, buffer);
                }

                if (buffer[0] == '\"') {
                    inLiteralContext = 1;
                }
            }
        }
        getNextToken(inputFile, buffer);
    }
}

void handleIf(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    handleConditionResult(inputFile, outputFile, hashMap, evalAndConsumeCondition(inputFile, outputFile, hashMap));
}

void handleIfDef(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];
    char value[80];
    char* mapVal;
    unsigned char check;
    
    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    strcpy(value, buffer);

    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    while (buffer[0] != '\n') {
        getNextToken(inputFile, buffer);
    }

    mapVal = getValue(hashMap, value);
    
    if (mapVal) {
        check = 1;
    }
    else {
        check = 0;
    }
    
    handleConditionResult(inputFile, outputFile, hashMap, check);
}

void handleIfNDef(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];
    char value[80];
    char* mapVal;
    unsigned char check;
    
    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    strcpy(value, buffer);

    do {
        getNextToken(inputFile, buffer);
    }
    while (buffer[0] == ' ');

    while (buffer[0] != '\n') {
        getNextToken(inputFile, buffer);
    }

    mapVal = getValue(hashMap, value);
    
    if (mapVal) {
        check = 0;
    }
    else {
        check = 1;
    }
    
    handleConditionResult(inputFile, outputFile, hashMap, check);
}

void handlePreprocessing(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap) {
    char buffer[80];

    getNextToken(inputFile, buffer);
    
    if (!strcmp(buffer, "define"))
    {
        handleDefine(inputFile, outputFile, hashMap);
    }
    else if(!strcmp(buffer, "include")) {
        handleInclude(inputFile, outputFile, hashMap);
    }
    else if(!strcmp(buffer, "undef"))
    {
        handleUndefine(inputFile, outputFile, hashMap);
    }
    else if(!strcmp(buffer, "if"))
    {
        handleIf(inputFile, outputFile, hashMap);
    }
    else if(!strcmp(buffer, "ifdef"))
    {
        handleIfDef(inputFile, outputFile, hashMap);
    }
    else if(!strcmp(buffer, "ifndef"))
    {
        handleIfNDef(inputFile, outputFile, hashMap);
    }
    else {
        while (buffer[0] != '\n') {
            getNextToken(inputFile, buffer);
        }
    }
}

void parse(FILE* inputFile, FILE* outputFile, struct HashMap* hashMap)
{
    char buffer[80];
    unsigned char inLiteralContext;
    char prevChr;
    char* value;

    inLiteralContext = 0;

    getNextToken(inputFile, buffer);
    
    while (buffer[0] != EOF) {

        if(!strcmp(buffer, "#"))
        {
            handlePreprocessing(inputFile, outputFile, hashMap);
        }
        else
        {
            if (inLiteralContext) {
                prevChr = '0';

                while (buffer[0] != '\"' && prevChr != '\\') {
                    processOutput(outputFile, buffer);
                    prevChr = buffer[0];
                    getNextToken(inputFile, buffer);
                }
                
                processOutput(outputFile, buffer);
                inLiteralContext = 0;
            }
            else {
                value = getValue(hashMap, buffer);
                
                if (value) {
                    processOutput(outputFile, value);
                }
                else {
                    processOutput(outputFile, buffer);
                }

                if (buffer[0] == '\"') {
                    inLiteralContext = 1;
                }
            }
        }
        getNextToken(inputFile, buffer);
    }
}

int main(int argc, char** argv)
{
    struct HashMap* hashMap;
    FILE *fp;
    FILE* fo;
    int i;
    unsigned char closeFile;
    char key[80];
    unsigned int itKey;
    char value[80];
    unsigned int itValue;
    unsigned char split;
    unsigned int itArg;
    char fName[80];

    closeFile = 0;
    fo = NULL;
    fp = NULL;
    
    hashMap = initHashMap();
    
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-D")) {
            strcpy(key, "");
            itKey = 0;
            strcpy(value, "");
            itValue = 0;
            split = 0;
            itArg = 0;
            
            i++;
            
            for (itArg = 0; itArg < strlen(argv[i]); itArg++) {
                if (argv[i][itArg] == '=') {
                    split = 1;
                    continue;
                }
                
                if (!split) {
                    key[itKey] = argv[i][itArg];
                    itKey++;
                }
                else {
                    value[itValue] = argv[i][itArg];
                    itValue++;
                }
            }
            
            key[itKey] = '\0';
            value[itValue] = '\0';
            
            setValue(hashMap, key, value);
        }
        else if (!strcmp(argv[i], "-I")) {
            i++;
            g_directories[g_directoriesN] = argv[i];
            g_directoriesN++;
        }
        else if (!strcmp(argv[i], "-o")) {
            i++;
            fo = fopen(argv[i], "w");
            closeFile = 1;
        }
        else {
            fp = fopen(argv[i], "r");
        }
    }
    
    if (!fp) {
        scanf("%s", fName);
        fp = fopen(fName, "r");
    }

    parse(fp, fo, hashMap);

    fclose(fp);

    if (closeFile) {
        fclose(fo);
    }

    freeHashMap(hashMap);

    return 0;
}
