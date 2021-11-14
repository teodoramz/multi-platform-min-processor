#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct ListNode
{
	char* key;
	char* value;
	struct ListNode* next;
};
void freeList(struct ListNode** head);
void freeNode(struct ListNode* node);