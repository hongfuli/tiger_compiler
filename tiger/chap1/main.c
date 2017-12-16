#include <stdio.h>
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

int main() {
    A_stm stm = prog();
    printf("the max args of all print stmt is %d", maxargs(stm));
}
