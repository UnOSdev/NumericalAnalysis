#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define TEST_PARSER
/* --- Data structures for tokens & RPN --- */
typedef enum {
    T_NUMBER, T_VAR,
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_POW,
    T_LPAREN, T_RPAREN,
    T_FUNC
} TokenType;

typedef struct {
    TokenType type;
    double   value;     // for numbers
    char     func[8];   // for functions, e.g. "sin"
} Token;

/* We'll keep the RPN in a global buffer: */
static Token *rpn = NULL;
static int    rpn_len = 0;

/* --- Utilities: stack for Tokens & ints --- */
typedef struct {
    Token *data; int top, cap;
} TStack;
static void ts_init(TStack *s){
    s->cap = 64; s->top = 0;
    s->data = malloc(s->cap * sizeof *s->data);
}
static void ts_push(TStack *s, Token t){
    if(s->top == s->cap) s->data = realloc(s->data, (s->cap*=2)*sizeof *s->data);
    s->data[s->top++] = t;
}
static Token ts_pop(TStack *s){ return s->data[--s->top]; }
static Token ts_peek(TStack *s){ return s->data[s->top-1]; }
static int  ts_empty(TStack *s){ return s->top == 0; }
static void ts_free(TStack *s){ free(s->data); }

/* --- Operator precedence & associativity --- */
static int prec(TokenType t){
    switch(t){
        case T_PLUS: case T_MINUS: return 1;
        case T_MUL:  case T_DIV:   return 2;
        case T_POW:               return 3;
        default:                  return 0;
    }
}
static int is_right_assoc(TokenType t){
    return t == T_POW;
}

/* --- Tokenizer: turns an input string into an array of tokens --- */
static Token *tokenize(const char *s, int *ntok){
    Token *out = NULL;
    int    cap = 0, n = 0;
    #define EMIT(t) do{     \
        if(n==cap) out = realloc(out, (cap = cap? cap*2:64)*sizeof *out);  \
        out[n++] = (t);     \
    } while(0)

    for(int i=0; s[i]; ){
        if(isspace(s[i])){ i++; continue; }
        // number
        if(isdigit(s[i]) || (s[i]=='.' && isdigit(s[i+1]))){
            char *end;
            double v = strtod(s+i, &end);
            Token t = { .type = T_NUMBER, .value = v };
            EMIT(t);
            i = end - s;
            continue;
        }
        // variable 'x'
        if(s[i]=='x'){
            Token t = { .type = T_VAR };
            EMIT(t);
            i++;
            continue;
        }
        // function or multi-letter
        if(isalpha(s[i])){
            char buf[8]; int len=0;
            while(isalpha(s[i]) && len<7) buf[len++] = s[i++];
            buf[len]=0;
            Token t = { .type = T_FUNC };
            strncpy(t.func, buf, 8);
            EMIT(t);
            continue;
        }
        // single‐char ops/paren
        Token t = {0};
        switch(s[i]){
            case '+': t.type=T_PLUS;  break;
            case '-': t.type=T_MINUS; break;
            case '*': t.type=T_MUL;   break;
            case '/': t.type=T_DIV;   break;
            case '^': t.type=T_POW;   break;
            case '(' :t.type=T_LPAREN;break;
            case ')' :t.type=T_RPAREN;break;
            default:
                fprintf(stderr,"Unexpected char '%c'\n", s[i]);
                exit(1);
        }
        EMIT(t);
        i++;
    }
    *ntok = n;
    return out;
}

/* --- Shunting-Yard: infix tokens → rpn global buffer --- */
static void to_rpn(Token *in, int nin){
    TStack opq; ts_init(&opq);
    free(rpn); rpn = NULL; rpn_len = 0; int cap=0;
    for(int i=0;i<nin;i++){
        Token tok = in[i];
        switch(tok.type){
            case T_NUMBER: case T_VAR:
                if(rpn_len==cap) rpn = realloc(rpn, (cap=cap?cap*2:64)*sizeof *rpn);
                rpn[rpn_len++] = tok;
                break;
            case T_FUNC:
                ts_push(&opq, tok);
                break;
            case T_PLUS: case T_MINUS:
            case T_MUL:  case T_DIV: case T_POW:
                while(!ts_empty(&opq)){
                    Token top = ts_peek(&opq);
                    if((top.type==T_FUNC) ||
                       (prec(top.type) > prec(tok.type)) ||
                       (prec(top.type)==prec(tok.type) && !is_right_assoc(tok.type)))
                    {
                        // pop
                        if(rpn_len==cap) rpn = realloc(rpn, (cap=cap?cap*2:64)*sizeof *rpn);
                        rpn[rpn_len++] = ts_pop(&opq);
                    } else break;
                }
                ts_push(&opq, tok);
                break;
            case T_LPAREN:
                ts_push(&opq, tok);
                break;
            case T_RPAREN:
                while(!ts_empty(&opq) && ts_peek(&opq).type!=T_LPAREN){
                    if(rpn_len==cap) rpn = realloc(rpn, (cap=cap?cap*2:64)*sizeof *rpn);
                    rpn[rpn_len++] = ts_pop(&opq);
                }
                if(ts_empty(&opq)){
                    fprintf(stderr,"Mismatched parentheses\n");
                    exit(1);
                }
                ts_pop(&opq);  // pop '('
                // if function on stack, pop it too
                if(!ts_empty(&opq) && ts_peek(&opq).type==T_FUNC){
                    if(rpn_len==cap) rpn = realloc(rpn, (cap=cap?cap*2:64)*sizeof *rpn);
                    rpn[rpn_len++] = ts_pop(&opq);
                }
                break;
        }
    }
    while(!ts_empty(&opq)){
        Token t2 = ts_pop(&opq);
        if(t2.type==T_LPAREN || t2.type==T_RPAREN){
            fprintf(stderr,"Mismatched parentheses\n");
            exit(1);
        }
        if(rpn_len==cap) rpn = realloc(rpn, (cap=cap?cap*2:64)*sizeof *rpn);
        rpn[rpn_len++] = t2;
    }
    ts_free(&opq);
}

/* --- RPN evaluation for a given x --- */
static double eval_rpn(double x){
    double *stack = malloc(rpn_len * sizeof *stack);
    int sp = 0;
    for(int i=0;i<rpn_len;i++){
        Token *t = &rpn[i];
        if(t->type == T_NUMBER){
            stack[sp++] = t->value;
        }
        else if(t->type == T_VAR){
            stack[sp++] = x;
        }
        else if(t->type == T_FUNC){
            if(sp < 1){ fprintf(stderr,"Stack underflow in func\n"); exit(1); }
            double v = stack[--sp], res;
            if     (!strcmp(t->func,"sin"))  res = sin(v);
            else if(!strcmp(t->func,"cos"))  res = cos(v);
            else if(!strcmp(t->func,"tan"))  res = tan(v);
            else if(!strcmp(t->func,"exp"))  res = exp(v);
            else if(!strcmp(t->func,"log"))  res = log(v);
            else if(!strcmp(t->func,"sqrt")) res = sqrt(v);
            else{
                fprintf(stderr,"Unknown func '%s'\n", t->func);
                exit(1);
            }
            stack[sp++] = res;
        }
        else {
            // binary op
            if(sp < 2){ fprintf(stderr,"Stack underflow in op\n"); exit(1); }
            double b = stack[--sp];
            double a = stack[--sp];
            double res = 0;
            switch(t->type){
                case T_PLUS:  res = a + b; break;
                case T_MINUS: res = a - b; break;
                case T_MUL:   res = a * b; break;
                case T_DIV:   res = a / b; break;
                case T_POW:   res = pow(a,b); break;
                default: break;
            }
            stack[sp++] = res;
        }
    }
    if(sp!=1){ fprintf(stderr,"RPN eval ended with %d elems\n", sp); exit(1); }
    double out = stack[0];
    free(stack);
    return out;
}

/* --- The user‐facing eval function --- */
static double _eval(double x){
    return eval_rpn(x);
}

/* --- Public API: parse and return pointer to _eval --- */
double (*parse_function(const char *expr))(double){
    int      ntok;
    Token   *in = tokenize(expr, &ntok);
    to_rpn(in, ntok);
    free(in);
    return &_eval;
}

/* --- EXAMPLE of how you'd use it: --- */
#ifdef TEST_PARSER
double bisection_meth(double x0, double x1, double eps, double(*func)(double)){
    double res0 = (*func)(x0);
    double res1 = (*func)(x1);
    double pivot;

    if (fabs(res0) <= eps)
    {
        return x0;
    }
    else if (fabs(res1) <= eps)
    {
        return x1;
    }
    else if (res0 * res1 >= 0)
    {
        printf("ERROR: Cannot detect a root between the intervals! (f(a) * f(b) is equal or bigger than 0)");
        return __INT64_MAX__ + 1;
    }
    do
    {
        pivot = (x0 + x1) / 2.0;
        double t = (*func)(pivot);
        if (t * res0 < 0)
            x1 = pivot;
        else if (t * res1 < 0)
        {
            x0 = pivot;
        }
        else
        {
            return pivot;
        }
        res0 = (*func)(x0);
        res1 = (*func)(x1);
#ifdef DEBUG
        printf("x0: %lf (%lf) x1: %lf (%lf) pivot: %lf (%lf)\n", x0, res0, x1, res1, pivot, t);
#endif
    } while (fabs(x0 - x1) > eps);
    return pivot;
}
int main(void){
    char expr[256];
    printf("Enter f(x): ");
    if(!fgets(expr, sizeof expr, stdin)) return 0;
    expr[strcspn(expr,"\n")] = 0;
    double (*f)(double) = parse_function(expr);

    double root = bisection_meth(-10, 10, 1e-6, f);
    printf("  → root ~= %.8f\n", root);
    return 0;
}
#endif