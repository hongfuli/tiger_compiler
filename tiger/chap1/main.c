#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "slp.h"
#include "prog1.h"

typedef enum {A_Type_Stmt, A_Type_Exp, A_Type_ExpList} A_NodeType;
int calPrintStmArgsCount(A_stm);
void traverse(void *node, A_NodeType tp, int *maxArgs); 

/*
 * 计算语句中所有的print语句中参数最多的那个，并返回其参数个数
 */
int maxargs(A_stm stm) {
    // 使用 CLR 顺序遍历树，当发现print语句时，统计其参数值，
    int maxArgs = 0;
    traverse(stm, A_Type_Stmt, &maxArgs);
    return maxArgs;
}

/*
 * 遍历整颗语法树，并在遇到print语句时计算其参数个数
 */
void traverse(void *node, A_NodeType tp, int *maxArgs) {
    if (A_Type_Stmt == tp) {
        A_stm stm = (A_stm)node;
        if (A_compoundStm == stm->kind) {
            traverse(stm->u.compound.stm1, A_Type_Stmt, maxArgs);
            traverse(stm->u.compound.stm2, A_Type_Stmt, maxArgs);
        } else if (A_assignStm == stm->kind) {
            traverse(stm->u.assign.exp, A_Type_Exp, maxArgs);
        } else if (A_printStm == stm->kind) {
            int args = calPrintStmArgsCount(stm);
            if (args > *maxArgs) {
                *maxArgs = args;
            }
            traverse(stm->u.print.exps, A_Type_ExpList, maxArgs);
        }
     } else if (A_Type_Exp == tp) {
            A_exp exp = (A_exp)node;
            if (A_opExp == exp->kind) {
                traverse(exp->u.op.left, A_Type_Exp, maxArgs);
                traverse(exp->u.op.right, A_Type_Exp, maxArgs);
            } else if (A_eseqExp == exp->kind) {
                traverse(exp->u.eseq.stm, A_Type_Stmt, maxArgs);
                traverse(exp->u.eseq.exp, A_Type_Exp, maxArgs);
            }
     } else if (A_Type_ExpList == tp) {
            A_expList expList = (A_expList)node;
            if (A_pairExpList == expList->kind) {
                traverse(expList->u.pair.head, A_Type_Exp, maxArgs);
                traverse(expList->u.pair.tail, A_Type_ExpList, maxArgs);
            } else if (A_lastExpList == expList->kind) {
                traverse(expList->u.last, A_Type_Exp, maxArgs);
            }
     }
}

int calPrintStmArgsCount(A_stm printStm) {
   int argsCount = 1;
   A_expList args = printStm->u.print.exps;
   while (args->kind == A_pairExpList) {
       argsCount++;
       args = args->u.pair.tail;
   }
   return argsCount;
}

typedef struct table *Table_;


/*
 * 变量表结构
 */
struct table {
    string id;
    int value;
    Table_ tail;
};


/*
 * 变量表构造函数
 */
Table_ Table(string id, int value, Table_ tail) {
    Table_ t = checked_malloc(sizeof(*t));
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
}


void printTable(Table_ t);

/*
 * 在表中查找变量值
 */
int lookup(Table_ t, string key) {
    while (t != NULL) {
        if (strcmp(key, t->id) == 0) {
            return t->value;
        }
        t = t->tail;
    }
    printf("the key is not exist: %s\n", key);
    return 0;
}

void printTable(Table_ t) {
    printf("the table is : ");
    while (t != NULL) {
        printf("%s=%d; ", t->id, t->value);
        t = t->tail;
    }
    printf("\n");
}

typedef struct _IntAndTable *IntAndTable_;

/*
 * 表达式执行结果结构
 */
struct _IntAndTable {
    int i;
    Table_ t;
};

IntAndTable_ IntAndTable(int i, Table_ t) {
    IntAndTable_ it = checked_malloc(sizeof(*it));
    it->i = i;
    it->t = t;
    return it;
}

Table_ interpStm(A_stm a, Table_ t);
IntAndTable_ interpExp(A_exp, Table_);
int oper(int, int, A_binop);

/*
 * 执行语句
 */
void interp(A_stm stm) {
    Table_ t = interpStm(stm, NULL);
    printTable(t);
}

Table_ interpStm(A_stm stm, Table_ t) {
    if (stm->kind == A_compoundStm) {
        t = interpStm(stm->u.compound.stm1, t);
        return interpStm(stm->u.compound.stm2, t);
    } else if (stm->kind == A_assignStm) {
        IntAndTable_ it = interpExp(stm->u.assign.exp, t);
        Table_ tt = Table(stm->u.assign.id, it->i, it->t);
        return tt;
    } else if(stm->kind == A_printStm) {
        A_expList exps = stm->u.print.exps;
        while (1) {
            if (exps->kind == A_lastExpList) {
                IntAndTable_ it = interpExp(exps->u.last, t);
                printf("%d ", it->i);
                t = it->t;
                break;
            } else if (exps->kind == A_pairExpList) {
                IntAndTable_ it = interpExp(exps->u.pair.head, t);
                printf("%d ", it->i);
                t = it->t;
                exps = exps->u.pair.tail;
            } else {
                printf("exps kind is unknown");
            }
        }
        printf("\n");
        return t;
    } else {
        printf("interpStm fail, the stm kind is unknown");
        exit(-1);
    }
}

IntAndTable_ interpExp(A_exp e, Table_ t) {
    if (e->kind == A_idExp) {
        return IntAndTable(lookup(t, e->u.id), t);
    } else if (e->kind == A_numExp) {
        return IntAndTable(e->u.num, t);
    } else if (e->kind == A_opExp) {
        IntAndTable_ it1 = interpExp(e->u.op.left, t);
        IntAndTable_ it2 = interpExp(e->u.op.right, it1->t);
        return IntAndTable(oper(it1->i, it2->i, e->u.op.oper), it2->t);
    } else if (e->kind == A_eseqExp) {
        t = interpStm(e->u.eseq.stm, t);
        return interpExp(e->u.eseq.exp, t);
    } else {
        printf("exp kind unknown");
        exit(-1);
    }

}

int oper(int i1, int i2, A_binop oper_t) {
    switch (oper_t) {
        case A_plus:
            return i1 + i2;
        case A_minus:
            return i1 - i2;
        case A_times:
            return i1 * i2;
        case A_div:
            return i1 / i2;
        default:
            printf("the oper sign unknown");
            exit(-1);
    }
}

int main() {
    A_stm stm = prog();
    printf("the max args of all print stmt is %d \n", maxargs(stm));

    stm = prog();
    printf("begin interp the stm: \n");
    interp(stm);
}
