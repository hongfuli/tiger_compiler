typedef struct A_stm_ *A_stm;
typedef struct A_exp_ *A_exp;
typedef struct A_expList_ *A_expList;
typedef enum {A_plus,A_minus,A_times,A_div} A_binop;

/* Stm define, has tree stm kinds:
  1. CompoundStm: Stm --> Stm ; Stm
  2. AssignStm: Stm --> id := Exp
  3. PrintStm: Stm --> print(ExpList)
 */
struct A_stm_ {enum {A_compoundStm, A_assignStm, A_printStm} kind;
             union {struct {A_stm stm1, stm2;} compound;
                    struct {string id; A_exp exp;} assign;
                    struct {A_expList exps;} print;
                   } u;
            };

// 如下是Stm的三种文法结构造器函数
A_stm A_CompoundStm(A_stm stm1, A_stm stm2);
A_stm A_AssignStm(string id, A_exp exp);
A_stm A_PrintStm(A_expList exps);

/* Exp define, has four kinds:
   1. IdExp: Exp --> id
   2. NumExp: Exp --> num
   3. OpExp: Exp --> Exp Binop Exp
   4. EseqExp: Exp --> (stm, Exp)
 */
struct A_exp_ {enum {A_idExp, A_numExp, A_opExp, A_eseqExp} kind;
             union {string id;
                    int num;
                    struct {A_exp left; A_binop oper; A_exp right;} op;
                    struct {A_stm stm; A_exp exp;} eseq;
                   } u;
            };

// 以下是Exp的四种文法结构的构造器函数
A_exp A_IdExp(string id);
A_exp A_NumExp(int num);
A_exp A_OpExp(A_exp left, A_binop oper, A_exp right);
A_exp A_EseqExp(A_stm stm, A_exp exp);

/* ExpList define, has two kinds:
   1. PairExpList: ExpList --> Exp, ExpList
   2. LastExpList: ExpList --> Exp
 */
struct A_expList_ {enum {A_pairExpList, A_lastExpList} kind;
                   union {struct {A_exp head; A_expList tail;} pair;
                          A_exp last;
                         } u;
                  };

// ExpList的两种文法结构构造器函数
A_expList A_PairExpList(A_exp head, A_expList tail);
A_expList A_LastExpList(A_exp last);

