
#define PUSH(arg) stack_push(stack, arg)

#define POP(arg)  stack_pop(stack, &arg)

#define ARG       argument

#define TMP1      tmp1

#define TMP2      tmp2

#define OUT(arg)  printf(stack_element_format, arg)

#define HLT       goto finish