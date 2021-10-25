/**
 * We use marco COM_DEF(name, value, code)
 * name  - name of command
 * value - number of format 0b00000000
 *                            ^^^
 *                            argMem argReg argImm
 * code  - what code to execute wwith command
 * 
 * Command list (with mask 0b00011111): Argument a and b from stack. b is popped first
 * 0x00 - htl   - stop sprogramm
 * 0x01 - push  - push arg to stack
 * 0x02 - pop   - pop  arg from stack
 * 0x03 - add   - a + b
 * 0x04 - sub   - a - b
 * 0x05 - mul   - a*b
 * 0x06 - div   - a/b
 * 0x07 - out   - print(a)
 * 0x08 - in    - scan() to stack
 * 0x09 - outc  - print(%c, a)
 * 0x0A - outf  - print(%f, a/b)
 * 0x0B - shl   - a << b
 * 0x0C - shr   - a >> b
 * 0x0D - sleep - make sleep for ms from stack
 * 0x0E - pause - pause untill user action
 * 0x0F - sqrt  - int sqrt(a);
 * 0x10 - jmp   - jump
 * 0x11 - ja    - jump a >  b
 * 0x12 - jae   - jump a >= b
 * 0x13 - jb    - jump a <  b
 * 0x14 - jbe   - jump a <= b
 * 0x15 - jeq   - jump a == b
 * 0x16 - jne   - jump a != b
 * 0x17 - 
 * 0x18 - call  - call function
 * 0x19 - ret   - return from function
 * 0x1A - aip   - add ip 
 * 0x1B - pushB - push 1 byte
 * 0x1C - 
 * 0x1D -
 * 0x1E - draw  - only if have VIDEO. displays from video mem
 * 0x1F - ERROR - Error command
 
 * 
 */


#ifndef COM_DEF
#error  COM_DEF not defined
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++
#define COM_DEF_ARITHM(name, num, arth, check)  \
    COM_DEF(name, num, {                        \
        POP(&TMP1);                             \
        POP(&TMP2);                             \
        {check};                                \
        PUSH(TMP2 arth TMP1);                   \
    })                                          \
//+++++++++++++++++++++++++++++++++++++++++++++++

COM_DEF(hlt,    0b00000000, {
    HLT;
})

COM_DEF(push,   0b11100001, {
    PUSH(*ARGPTR);
})

COM_DEF(pop,    0b11100010, {
    POP(ARGPTR);
})

COM_DEF_ARITHM(add,    0b00000011, +, ;)

COM_DEF_ARITHM(sub,    0b00000100, -, ;)

COM_DEF_ARITHM(mul,    0b00000101, *, ;)

//+++++++++++++++++++++++++++++++++++++++++++++++++++
COM_DEF_ARITHM(div,    0b00000110, +, {             \
    if(!TMP2){                                      \
        LOG_MESSAGE_F(ERROR, "Zero Division error");\
        ERR;                                        \
    }})                                             \
//+++++++++++++++++++++++++++++++++++++++++++++++++++

COM_DEF(out,    0b00000111, {
    POP(&TMP1);
    OUT(TMP1);
})

COM_DEF(in ,    0b00001000, {
    IN(&TMP1);
    PUSH(TMP1);
})

COM_DEF(outc,   0b00001001, {
    POP(&TMP1);
    OUTC(TMP1);
})

COM_DEF(outf,   0b00001010, {
    POP(&TMP1);
    POP(&TMP2);
    if(TMP2 == 0){
        LOG_MESSAGE_F(ERROR, "Zero Division error");
        ERR;
    }
    OUTF(TMP2, TMP1);
})

COM_DEF_ARITHM(shl,    0b00001011, <<, ;)

COM_DEF_ARITHM(shr,    0b00001100, >>, ;)

COM_DEF(wait,    0b00001101, {
    PAUSE;
})

COM_DEF(slp,    0b00001110, {
    POP(&TMP1)
    SLEEP(TMP1);
})

COM_DEF(sqrt,   0b00001111, {
    POP(&TMP1);
    if(TMP1 < 0)
        ERR;
    PUSH(round(sqrt(TMP1)));
})

COM_DEF(jmp,    0b01110000, {
    JMP(ARG);
})

#define COM_DEF_JMP(name, value, mark)  \
    COM_DEF(name, value, {              \
        POP(&TMP1);                     \
        POP(&TMP2);                     \
        if(TMP2 mark TMP1){             \
            JMP(ARG);}                  \
    })                                  \

COM_DEF_JMP(ja , 0b01110001, > )

COM_DEF_JMP(jae, 0b01110010, >=)

COM_DEF_JMP(jb , 0b01110011, < )

COM_DEF_JMP(jbe, 0b01110100, <=)

COM_DEF_JMP(je,  0b01110101, ==)

COM_DEF_JMP(jne, 0b01110110, !=)

#undef COM_DEF_JMP

COM_DEF(call, 0b00111000, {
    PUSH(IP);
    JMP(ARG);
})

COM_DEF(ret, 0b00011001, {
    POP(&TMP1);
    JMP_ABS(TMP1);
})

COM_DEF(aip,  0b11111010, {
    PUSH(IP + ARG);
})

COM_DEF(pushB, 0b11111011, {
    PUSH(ARG & 0xFF);
})

COM_DEF(draw, 0b00011110, {
    #ifdef VIDEO
    DRAW;
    #else
    ERR;
    #endif
})

#undef COM_DEF_ARITHM
