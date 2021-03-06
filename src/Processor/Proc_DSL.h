
//+++++++++++++++++++++++++++++++++++++++++++++++
#define PUSH(arg) {                             \
    stack_push(&proc->stack, arg);              \
    if(proc->stack.size > STACK_MAX_SZ)         \
        return RuntimeError::StackOverflow;     \
        }                                       \
//+++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++
#define POP(argptr) {                           \
    if(proc->stack.size == 0){                  \
        return RuntimeError::EmptyStack;        \
        }                                       \
    stack_pop(&proc->stack, argptr);            \
    }while(0)                                   \
//+++++++++++++++++++++++++++++++++++++++++++++++

#define ARGPTR          argument

#define ARG             (*ARGPTR)

#define TMP1            tmp1

#define TMP2            tmp2

#define OUT(arg)        printf("%d ", arg)

#define IP              proc->ip.asArg

#define HLT             proc->status = ProcStatus::Halted

#define IN(argptr)      scanf("%d ", argptr)

#define JMP(arg)        proc->ip.asArg += arg

#define JMP_ABS(arg)    proc->ip.asArg = arg

#define OUTC(arg)       printf("%c", (char)arg);

#define PAUSE           getchar();

#define SLEEP(arg)      sleep(arg);

#define OUTF(val, prec) printf("%.3f ",((double)val)/prec)

#define ERR             proc->status = ProcStatus::Error

#define DRAW            draw(&proc->videoDriver)