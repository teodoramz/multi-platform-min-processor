#include "listNode.h"

#define MAX_ELEMENTS_SIZE 15

struct HashMap{
    struct ListNode* elements[MAX_ELEMENTS_SIZE];
};

struct HashMap* initHashMap(void);
void freeHashMap(struct HashMap*);
unsigned int computeHash(char*);
void setValue(struct HashMap*, char*, char*);
char* getValue(struct HashMap*, char*);
void deleteValue(struct HashMap* hashMap, char* key);