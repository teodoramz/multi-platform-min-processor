#define _CRT_SECURE_NO_WARNINGS
#include "hashMap.h"

struct HashMap* initHashMap(void)
{
    int i;
    struct HashMap* newHashMap;

    i = 0;
    newHashMap = (struct HashMap*)malloc(sizeof(struct HashMap));
    if (!newHashMap)
        exit(12);

    for (i = 0; i < MAX_ELEMENTS_SIZE; i++)
    {
        newHashMap->elements[i] = NULL;
    }

    return newHashMap;
}

void freeHashMap(struct HashMap* hashMap)
{
    int i;
    i = 0;

    for (i = 0; i < MAX_ELEMENTS_SIZE; i++)
    {
        freeList(&hashMap->elements[i]);
    }

    free(hashMap);
}

unsigned int computeHash(char* key)
{
    unsigned int sum;
    int i;
    sum = 0;
    i = 0;
    for (i = 0; i < strlen(key); i++)
    {
        sum += key[i];
    }

    return sum % MAX_ELEMENTS_SIZE;
}

struct ListNode* getListNodeByKey(struct HashMap* hashMap, char* key) {
    int hashKey;
    struct ListNode* listItr;

    hashKey = computeHash(key);
    listItr = hashMap->elements[hashKey];

    if (!listItr)
        return NULL;

    if (!strcmp(listItr->key, key)) {
        return listItr;
    }
    
    while (listItr->next) {
        if (!strcmp(listItr->key, key)) {
            return listItr;
        }
        
        listItr = listItr->next;
    }
    
    return NULL;
}

void setValue(struct HashMap* hashMap, char* key, char* value)
{
    unsigned int hashKey;
    struct ListNode* newValue;
    struct ListNode* listItr;

    hashKey = computeHash(key);
    listItr = NULL;

    if ((newValue = getListNodeByKey(hashMap, key))) {
        free(newValue->value);
        newValue->value = malloc(sizeof(value));
        strcpy(newValue->value, value);
        
        return;
    }
    
    newValue = (struct ListNode*)malloc(sizeof(struct ListNode));
    if (!newValue)
        exit(12);

    newValue->key = malloc(sizeof(key));
    newValue->value = malloc(sizeof(value));
    strcpy(newValue->key, key);
    strcpy(newValue->value, value);
    
    if (hashMap->elements[hashKey] == NULL) {
        hashMap->elements[hashKey] = newValue;
        hashMap->elements[hashKey]->next = NULL;
        
        return;
    }
    
    listItr = hashMap->elements[hashKey];
    while (listItr->next) {
        listItr = listItr->next;
    }
    
    listItr->next = newValue;
    listItr->next->next = NULL;
}

char* getValue(struct HashMap* hashMap, char* key)
{
    int hashKey;
    struct ListNode* listItr;

    hashKey = computeHash(key);
    listItr = hashMap->elements[hashKey];

    if (!listItr) {
        return NULL;
    }
    
    if (!strcmp(listItr->key, key)) {
        return listItr->value;
    }
    
    while (listItr->next) {
        if (!strcmp(listItr->key, key)) {
            return listItr->value;
        }
        
        listItr = listItr->next;
    }
    
    return NULL;
}

void deleteValue(struct HashMap* hashMap, char* key)
{
    int hashKey;
    struct ListNode* listItr;
    struct ListNode* prevlistItr;

    hashKey = computeHash(key);
    listItr = hashMap->elements[hashKey];
    prevlistItr = NULL;

    if (!listItr) {
        return;
    }

    do {
        if (!strcmp(listItr->key, key)) {
            if (prevlistItr) {
                prevlistItr->next = listItr->next;
            }
            else {
                hashMap->elements[hashKey] = listItr->next;
            }

            freeNode(listItr);
            return;
        }

        prevlistItr = listItr;
        listItr = listItr->next;
    }
    while (listItr->next);
}
