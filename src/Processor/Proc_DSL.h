
#define PUSH(arg)   stack_push(&proc->stack, arg)

#define POP(argptr) stack_pop(&proc->stack, argptr)

#define ARGPTR      argument

#define ARG         (*ARGPTR)

#define TMP1        tmp1

#define TMP2        tmp2

#define OUT(arg)    printf("%d\n", arg)

#define IP          proc->ip.asArg

#define HLT         proc->status = ProcStatus::Halted

#define IN(argptr)  scanf("%d", argptr)

#define JMP(arg)    proc->ip.asArg = arg