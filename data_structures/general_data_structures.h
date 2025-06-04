#if !defined(G_DATA_STRUCTURES_H)
#define G_DATA_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef enum{
    OP_ERROR = -1,
    OP_FAILURE = 0,
    OP_SUCCESS = 1
} OperationResult;

typedef struct sLLNode{
    const void *data;
    //const DataType type;
    const unsigned element_size;
    struct sLLNode *next;
} *LLNode;

LLNode initNode(unsigned element_size){
    if(element_size == 0){
        printf("An element cannot be of size 0 for initNode()! NULL returned.\n");
        return NULL;
    }
    LLNode node = malloc(sizeof(struct sLLNode));
    unsigned *ref = &node->element_size;
    *ref = element_size;
    node->next = NULL;
    node->data = NULL;
    return node;
}

void freeNode(LLNode *node){
    if(*node == NULL)
        return;
    if((*node)->data != NULL)
        free((*node)->data);
    (*node)->data = NULL;
    (*node)->next = NULL;
    free((*node));
    *node = NULL;
}

void pushNode(LLNode node, void *source){
    if(node == NULL){
        printf("NULL node provided for pushNode()! Ignoring request.\n");
        return;
    }else if(node->data == NULL){
        node->data = malloc(sizeof(node->element_size));
        memcpy(node->data, source, node->element_size);
        node->next = NULL;
    }else{
        while(node->next != NULL)
            node = node->next;
        LLNode new = initNode(node->element_size);
        pushNode(new, source);
        node->next = new;
    }
}

void popNode(LLNode node, void *destination){
    if(node == NULL || node->data == NULL){
        printf("NULL node provided for popNode()! Ignoring request.\n");
    }else if(destination == NULL){
        printf("NULL destination provided for popNode()! Ignoring request.\n");
    }else if(node->next == NULL){ //Only 1 element in the linked list
        memcpy(destination, node->data, node->element_size);
        // Custom free-ing 
        free(node->data);
        node->data = NULL;
        node->next = NULL;
    }else{
        LLNode head = node;
        while(node->next->next != NULL){
            node = node->next;
        }
        memcpy(destination, node->next->data, node->next->element_size);
        free(node->next->data);
        free(node->next);
        node->next = NULL;
    }
}

typedef struct sStack{
    void *data;
    int top;
    const unsigned element_size;
    const unsigned capacity;
} Stack;

Stack initStack(unsigned element_size, unsigned capacity){
    return (Stack){
        .data = malloc(capacity * element_size),
        .top = -1,
        .element_size = element_size,
        .capacity = capacity,
    };
}

void freeStack(Stack *stack){
    if(stack == NULL || stack->data == NULL){
        printf("NULL stack provided for freeStack()! Request ignored.\n");
        return;
    }
    free(stack->data);
    stack->data = NULL;
    unsigned *ref = &stack->capacity;
    *ref = 0;
    ref = &stack->element_size;
    *ref = 0;
}

OperationResult pushStack(Stack *stack, void *source){
    if(stack == NULL || stack->data == NULL){
        #ifdef DEBUG printf("NULL stack provided for pushStack()! Request ignored.\n"); 
        #endif
        return OP_ERROR;
    }else if(source == NULL){
        printf("NULL source provided for pushStack()! Request ignored.\n");
        return OP_ERROR;
    }else if(stack->top >= ((int)stack->capacity - 1)){
        printf("Stack is full for pushStack()! Request ignored.\n");
        return OP_FAILURE;
    }
    char *ptr = (char*)stack->data;// Because char is 1 byte
    memcpy(&ptr[stack->element_size * (++stack->top)], source, stack->element_size);
    return OP_SUCCESS;
}

OperationResult popStack(Stack *stack, void *destination){
    if(stack == NULL || stack->element_size == 0 || stack->data == NULL){
        printf("NULL stack provided for peekStack()! Request ignored.\n");
        return OP_ERROR;
    }
    else if(destination == NULL){
        printf("NULL destination provided for peekStack()! Request ignored.\n");
        return OP_ERROR;
    }
    else if(stack->top < 0){
        printf("Empty stack provided for peekStack()! Request ignored.\n");
        return OP_FAILURE;
    }

    char *ptr = (char*)stack->data;// Because char is 1 byte
    memcpy(destination, &ptr[stack->element_size * stack->top--], stack->element_size);    
    return OP_SUCCESS;
}

OperationResult peekStack(Stack *stack, void *destination){
    if(stack == NULL || stack->element_size == 0 || stack->data == NULL){
        printf("NULL stack provided for popStack()! Request ignored.\n");
        return OP_ERROR;
    }else if(stack->top < 0){
        printf("Empty stack provided for popStack()! Request ignored.\n");
        return OP_FAILURE;
    }

    char *ptr = (char*)stack->data;// Because char is 1 byte
    memcpy(destination, &ptr[stack->element_size * stack->top], stack->element_size);
    return OP_SUCCESS;
}


typedef struct sBinaryTree{
    struct sBinaryTree *parent;

    struct sBinaryTree *left_child;
    struct sBinaryTree *right_child;

    const unsigned element_size;
    void *data;
} *BiTree;

BiTree initTree(const unsigned element_size, void *source){
    if(source == NULL)
        return NULL;
    
    BiTree root = malloc(sizeof(struct sBinaryTree));
    root->left_child = NULL;
    root->right_child = NULL;
    root->parent = NULL;
    unsigned *ptr = &root->element_size;
    *ptr = element_size;
    memcpy(root->data, source, element_size);
    return root;
}


bool isEmptyTree(BiTree tree){
    if(tree == NULL) 
        return true;
    return false;
}

BiTree getLeftTree(BiTree tree){
    if(isEmptyTree(tree))
        return NULL;
    return tree->left_child;
}

BiTree getRightTree(BiTree tree){
    if(isEmptyTree(tree))
        return NULL;
    return tree->right_child;
}

void addLeftTree(BiTree tree, void *source){
    if(isEmptyTree(tree))
        return;

    tree->left_child = initTree(tree->element_size, source);
    tree->left_child->parent = tree;
}

void addRightTree(BiTree tree, void *source){
    if(isEmptyTree(tree))
        return;

    tree->right_child = initTree(tree->element_size, source);
    tree->right_child->parent = tree;
}

void visitTree(BiTree tree){
    if(isEmptyTree(tree) == false) 
        printf("%p ", tree->data);
}
//In-Order Traversal
void lvr(BiTree tree){
    if(isEmptyTree(tree) == false){
        lvr(tree->left_child);
        visitTree(tree);
        lvr(tree->right_child);
    }
}
//In-Order Traversal
void rvl(BiTree tree){
    if(isEmptyTree(tree) == false){
        rvl(tree->right_child);
        visitTree(tree);
        rvl(tree->left_child);
    }
}
//Pre-Order Traversal
void vlr(BiTree tree){
    if(isEmptyTree(tree) == false){
        visitTree(tree);
        vlr(tree->left_child);
        vlr(tree->right_child);
    }
}
//Pre-Order Traversal
void vrl(BiTree tree){
    if(isEmptyTree(tree) == false){
        visitTree(tree);
        vrl(tree->right_child);
        vrl(tree->left_child);
    }
}
//Post-Order Traversal
void lrv(BiTree tree){
    if(isEmptyTree(tree) == false){
        lrv(tree->left_child);
        lrv(tree->right_child);
        visitTree(tree);
    }
}
//Post-Order Traversal
void rlv(BiTree tree){
    if(isEmptyTree(tree) == false){
        rlv(tree->right_child);
        rlv(tree->left_child);
        visitTree(tree);
    }
}

#endif // G_DATA_STRUCTURES_H
