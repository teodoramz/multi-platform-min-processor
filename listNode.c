#include "listNode.h"

void freeNode(struct ListNode* node) {
    free(node->key);
    free(node->value);
    free(node);

    node = NULL;
}

void freeList(struct ListNode** head){
    struct ListNode* currentNode;
    struct ListNode* prevNode;
    currentNode = *head;
    prevNode = NULL;
    
    if (!currentNode)
        return;

    while (currentNode->next != NULL){
        if (prevNode) {
            freeNode(prevNode);
        }
        
        prevNode = currentNode;
        currentNode = currentNode->next;
    }

    if (currentNode) {
        freeNode(currentNode);
    }
    
    *head = NULL;
}
