#if !defined(PARSER_H)
#define PARSER_H

#include "../data_structures/general_data_structures.h"
#include <ctype.h>

typedef enum{
    TOK_FUN = 0b01000000, //A function. (sin, cos, log etc.)
    TOK_RES = 0b10000000, //A result means that it's a constant/variable.
    
    TOK_ADD = '+',
    TOK_SUB = '/',
    TOK_MUL = '*',
    TOK_ARG = '(' | ')',
    TOK_POW = '^',

} TokenType;

typedef enum{
    FUN_SIN = 1,
    FUN_COS,
    FUN_COT,
    FUN_TAN,
    FUN_LOG
} FuncType;



typedef struct{
    TokenType type;
    FuncType function;
    unsigned log_base;
    double value;
} Token;

int priority(char ch){
    switch (ch)
    {
    case '^':
        return 3;
        break;
    case '/':
    case '*':
        return 2;
    case '+':
    case '-':
        return 1;
    default:
        return -1;
        break;
    }
}

char* infix_to_postfix(const char *infix) {
    if (!infix) {
        fprintf(stderr, "NULL input string in infix_to_postfix()\n");
        return NULL;
    }

    const size_t length = strlen(infix);
    if (length == 0) {
        return strdup(""); // Return empty string for empty input
    }

    // Worst-case: All characters are operators/parentheses (e.g., "(((")
    char *postfix = malloc(2 * length + 1); // Extra space for safety
    if (!postfix) {
        perror("malloc failed in infix_to_postfix()");
        return NULL;
    }

    Stack stack = initStack(sizeof(char), length);
    if (!stack.data) {
        free(postfix);
        perror("Stack initialization failed in infix_to_postfix()");
        return NULL;
    }

    size_t postfix_index = 0;
    for (size_t i = 0; i < length; i++) {
        const char ch = infix[i];

        if (isalnum(ch)) { // Operand (number/variable)
            postfix[postfix_index++] = ch;
        }
        else if (ch == '(') { // Opening parenthesis
            if (pushStack(&stack, &ch) != OP_SUCCESS) {
                free(postfix);
                freeStack(&stack);
                return NULL;
            }
        }
        else if (ch == ')') { // Closing parenthesis
            char top_op;
            while (peekStack(&stack, &top_op) == OP_SUCCESS && top_op != '(') {
                if (popStack(&stack, &top_op)) {
                    break; // Should not happen if peek succeeded
                }
                postfix[postfix_index++] = top_op;
            }

            // Check for mismatched parentheses
            if (top_op != '(') {
                fprintf(stderr, "Mismatched parentheses in infix_to_postfix()\n");
                free(postfix);
                freeStack(&stack);
                return NULL;
            }
            popStack(&stack, &top_op); // Discard '('
        }
        else { // Operator (+, -, *, /, ^)
            char top_op;
            while (peekStack(&stack, &top_op) == OP_SUCCESS && 
                   top_op != '(' && 
                   priority(ch) <= priority(top_op)) {
                popStack(&stack, &top_op);
                postfix[postfix_index++] = top_op;
            }
            pushStack(&stack, &ch);
        }
    }

    // Pop remaining operators
    char op;
    while (popStack(&stack, &op) == OP_SUCCESS) {
        if (op == '(') {
            fprintf(stderr, "Mismatched parentheses in infix_to_postfix()\n");
            free(postfix);
            freeStack(&stack);
            return NULL;
        }
        postfix[postfix_index++] = op;
    }

    postfix[postfix_index] = '\0'; // Null-terminate
    freeStack(&stack);
    return postfix;
}



double solve_simple_math(const char *str){
    const unsigned length = strlen(str);
    if(length < 1){
        return 0.0;
    }
    Stack trees = initStack(sizeof(BiTree), 10);
    unsigned index = 0;
    
    while(index < length){
        char chr = str[index];
        switch (chr){
            Token tok = { .type = chr };
            BiTree new = initTree(sizeof(Token), &tok);
        case '+':
        case '-':
            
            
            break;
        case '*':
        case '/':
            break;
        case '^':
            break;
        case '(':
            
            break;
        
        case ')':

            break;
        default:
            if(isalnum(chr)){
                for (size_t i = index + 1; i < length; i++)
                {
                    if(str[i]);
                }
                
            }
            break;
        }
    }

}


#endif // PARSER_H
