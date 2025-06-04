#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

/* --- Data structures for tokens & RPN --- */
typedef enum {
    T_NUMBER, T_VAR,
    T_PLUS, T_MINUS, T_MUL, T_DIV, T_POW,
    T_LPAREN, T_RPAREN,
    T_FUNC
} TokenType;

typedef struct {
    TokenType type;
    double   value;
    char     func[8];
} Token;

static Token *rpn = NULL;
static int    rpn_len = 0;

/* --- Stack for tokens --- */
typedef struct { Token *data; int top, cap; } TStack;
static void ts_init(TStack *s) { s->cap = 64; s->top = 0; s->data = malloc(s->cap * sizeof *s->data); }
static void ts_push(TStack *s, Token t) { if (s->top == s->cap) s->data = realloc(s->data, (s->cap *= 2) * sizeof *s->data); s->data[s->top++] = t; }
static Token ts_pop(TStack *s) { return s->data[--s->top]; }
static Token ts_peek(TStack *s) { return s->data[s->top - 1]; }
static int  ts_empty(TStack *s) { return s->top == 0; }
static void ts_free(TStack *s) { free(s->data); }

/* --- Precedence and associativity --- */
static int prec(TokenType t) {
    switch (t) {
        case T_PLUS: case T_MINUS: return 1;
        case T_MUL: case T_DIV:    return 2;
        case T_POW:               return 3;
        default:                  return 0;
    }
}
static int is_right_assoc(TokenType t) { return t == T_POW; }

/* --- Tokenizer --- */
static Token *tokenize(const char *s, int *ntok) {
    Token *out = NULL; int cap = 0, n = 0;
    TokenType last = T_LPAREN;

    #define EMIT(tok) do {                 \
        if (n == cap)                     \
            out = realloc(out, (cap ? cap*2 : 64) * sizeof *out); \
        out[n] = (tok);                   \
        last = out[n].type;               \
        n++;                              \
    } while (0)

    for (int i = 0; s[i]; ) {
        if (isspace((unsigned char)s[i])) { i++; continue; }
        int can_be_signed = (last == T_PLUS || last == T_MINUS || last == T_MUL || last == T_DIV || last == T_POW || last == T_LPAREN);
        // number with optional sign
        if (((s[i] == '+' || s[i] == '-') && can_be_signed && isdigit((unsigned char)s[i+1]))
            || isdigit((unsigned char)s[i])
            || (s[i] == '.' && isdigit((unsigned char)s[i+1]))) {
            char *end;
            double v = strtod(s + i, &end);
            Token t = { .type = T_NUMBER, .value = v };
            EMIT(t);
            i = end - s;
            continue;
        }
        // variable or constant
        if (s[i] == 'x' || s[i] == 'e') {
            Token t = { .type = s[i] == 'x' ? T_VAR : T_NUMBER, .value = s[i] == 'e' ? M_E : 0 };
            EMIT(t);
            i++;
            continue;
        }
        // function name
        if (isalpha((unsigned char)s[i])) {
            char buf[8]; int len = 0;
            while (isalpha((unsigned char)s[i]) && len < 7) buf[len++] = s[i++];
            buf[len] = 0;
            Token t = { .type = T_FUNC };
            strncpy(t.func, buf, sizeof t.func);
            EMIT(t);
            continue;
        }
        // operators and parens
        Token t = {0};
        switch (s[i]) {
            case '+': t.type = T_PLUS;  break;
            case '-': t.type = T_MINUS; break;
            case '*': t.type = T_MUL;   break;
            case '/': t.type = T_DIV;   break;
            case '^': t.type = T_POW;   break;
            case '(': t.type = T_LPAREN;break;
            case ')': t.type = T_RPAREN;break;
            default:
                fprintf(stderr, "Unexpected '%c'\n", s[i]); exit(1);
        }
        EMIT(t);
        i++;

        // implicit multiplication
        if (n > 0) {
            TokenType just = out[n-1].type;
            char next = s[i];
            if ((just == T_NUMBER || just == T_VAR || just == T_RPAREN)
             && (isdigit((unsigned char)next) || next=='x' || next=='e' || next=='(' || isalpha((unsigned char)next))) {
                Token m = { .type = T_MUL };
                EMIT(m);
            }
        }
    }
    *ntok = n;
    return out;
}

/* --- Shunting-Yard → RPN --- */
static void to_rpn(Token *in, int nin) {
    TStack st; ts_init(&st);
    free(rpn); rpn = NULL; rpn_len = 0; int cap = 0;
    for (int i = 0; i < nin; i++) {
        Token tok = in[i];
        if (tok.type == T_NUMBER || tok.type == T_VAR) {
            if (rpn_len == cap) rpn = realloc(rpn, (cap ? cap*2 : 64) * sizeof *rpn);
            rpn[rpn_len++] = tok;
        } else if (tok.type == T_FUNC) {
            ts_push(&st, tok);
        } else if (tok.type == T_PLUS || tok.type == T_MINUS || tok.type == T_MUL || tok.type == T_DIV || tok.type == T_POW) {
            while (!ts_empty(&st)) {
                Token top = ts_peek(&st);
                if (top.type == T_FUNC || prec(top.type) > prec(tok.type) || (prec(top.type) == prec(tok.type) && !is_right_assoc(tok.type))) {
                    if (rpn_len == cap) rpn = realloc(rpn, (cap ? cap*2 : 64) * sizeof *rpn);
                    rpn[rpn_len++] = ts_pop(&st);
                } else break;
            }
            ts_push(&st, tok);
        } else if (tok.type == T_LPAREN) {
            ts_push(&st, tok);
        } else if (tok.type == T_RPAREN) {
            while (!ts_empty(&st) && ts_peek(&st).type != T_LPAREN) {
                if (rpn_len == cap) rpn = realloc(rpn, (cap ? cap*2 : 64) * sizeof *rpn);
                rpn[rpn_len++] = ts_pop(&st);
            }
            if (ts_empty(&st)) { fprintf(stderr,"Mismatched parentheses\n"); exit(1); }
            ts_pop(&st);
            if (!ts_empty(&st) && ts_peek(&st).type == T_FUNC) {
                if (rpn_len == cap) rpn = realloc(rpn, (cap ? cap*2 : 64) * sizeof *rpn);
                rpn[rpn_len++] = ts_pop(&st);
            }
        }
    }
    while (!ts_empty(&st)) {
        Token t = ts_pop(&st);
        if (t.type == T_LPAREN || t.type == T_RPAREN) { fprintf(stderr,"Mismatched parentheses\n"); exit(1); }
        if (rpn_len == cap) rpn = realloc(rpn, (cap ? cap*2 : 64) * sizeof *rpn);
        rpn[rpn_len++] = t;
    }
    ts_free(&st);
}

/* --- Evaluate RPN --- */
static double eval_rpn(double x) {
    double *stk = malloc(rpn_len * sizeof *stk);
    int sp = 0;
    for (int i = 0; i < rpn_len; i++) {
        Token *t = &rpn[i];
        if (t->type == T_NUMBER) stk[sp++] = t->value;
        else if (t->type == T_VAR) stk[sp++] = x;
        else if (t->type == T_FUNC) {
            if (sp < 1) { fprintf(stderr,"Stack underflow in func\n"); exit(1); }
            double v = stk[--sp], r;
            if (!strcmp(t->func,"sin")) r = sin(v);
            else if (!strcmp(t->func,"cos")) r = cos(v);
            else if (!strcmp(t->func,"tan")) r = tan(v);
            else if (!strcmp(t->func,"exp")) r = exp(v);
            else if (!strcmp(t->func,"log")) r = log(v);
            else if (!strcmp(t->func,"sqrt")) r = sqrt(v);
            else { fprintf(stderr,"Unknown func '%s'\n", t->func); exit(1); }
            stk[sp++] = r;
        } else {
            if (sp < 2) { fprintf(stderr,"Stack underflow in op\n"); exit(1); }
            double b = stk[--sp], a = stk[--sp], r = 0;
            switch (t->type) {
                case T_PLUS:  r = a + b; break;
                case T_MINUS: r = a - b; break;
                case T_MUL:   r = a * b; break;
                case T_DIV:   r = a / b; break;
                case T_POW:   r = pow(a, b); break;
                default: break;
            }
            stk[sp++] = r;
        }
    }
    if (sp != 1) { fprintf(stderr,"RPN eval ended with %d elems\n", sp); exit(1); }
    double out = stk[0]; free(stk);
    return out;
}

/* --- Public API --- */
static double _eval(double x) { return eval_rpn(x); }
double (*parse_function(const char *expr))(double) {
    int ntok;
    Token *tokens = tokenize(expr, &ntok);
    to_rpn(tokens, ntok);
    free(tokens);
    return &_eval;
}

/* --- EXAMPLE of how you'd use it: --- */
#ifdef TEST_PARSER
double bisection_meth(double, double, double, double(*)(double));  /* your code */
int main(void){
    char expr[256];
    printf("Enter f(x): ");
    if(!fgets(expr, sizeof expr, stdin)) return 0;
    expr[strcspn(expr,"\n")] = 0;
    double (*f)(double) = parse_function(expr);

    double root = bisection_meth(0, 2, 1e-6, f);
    printf("  → root ~= %.8f\n", root);
    return 0;
}

#endif
// --- these mirror your parser's static rpn[] and rpn_len ---
extern Token *rpn;        // declared in parser.c
extern int    rpn_len;    // declared in parser.c

static Token *rpn_f = NULL;
static int    rpn_len_f;

static Token *rpn_fder = NULL;
static int    rpn_len_fder;

// copy the parser's current RPN into rpn_f / rpn_fder
static void save_current_rpn(Token **dst, int *dst_len) {
    if (*dst) free(*dst);
    *dst_len = rpn_len;
    *dst = malloc(rpn_len * sizeof(Token));
    memcpy(*dst, rpn, rpn_len * sizeof(Token));
}

// swap in a saved RPN before calling eval_rpn()
static double call_saved_rpn(Token *src, int src_len, double x) {
    // swap
    Token *old_rpn   = rpn;
    int    old_len   = rpn_len;
    rpn     = src;
    rpn_len = src_len;
    // eval
    double y = eval_rpn(x);
    // restore
    rpn     = old_rpn;
    rpn_len = old_len;
    return y;
}

static double call_f(double x) {
    return call_saved_rpn(rpn_f,   rpn_len_f,   x);
}

static double call_fder(double x) {
    return call_saved_rpn(rpn_fder, rpn_len_fder, x);
}
