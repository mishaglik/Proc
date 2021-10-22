/**
 * We use marco COM_DEF(name, value, code)
 * name  - name of command
 * value - number of format 0b00000000
 *                            ^^^
 *                             ||
 * TODO: Documentation  
 * ARG
 * PUSH()
 * POP()
 * 
 */


#ifndef COM_DEF
#error  COM_DEF not defined
#endif

COM_DEF(hlt,    0b00000000, {
    HLT;
})

COM_DEF(push,   0b11100001, {
    PUSH(*ARGPTR);
})

COM_DEF(pop,    0b11100010, {
    POP(ARGPTR);
})

COM_DEF(add,    0b00000011, {
    POP(&TMP1);
    POP(&TMP2);
    PUSH(TMP1 + TMP2);
})

COM_DEF(sub,    0b00000100, {
    POP(&TMP1);
    POP(&TMP2);
    PUSH(TMP1 - TMP2);
})

COM_DEF(mul,    0b00000101, {
    POP(&TMP1);
    POP(&TMP2);
    PUSH(TMP1 * TMP2);
})

COM_DEF(div,    0b00000110, {
    POP(&TMP1);
    POP(&TMP2);
    if(!TMP2){
        LOG_MESSAGE_F(ERROR, "Zero Division error");
        HLT;
    }
    PUSH(TMP1 / TMP2);
})

COM_DEF(out,    0b00000111, {
    POP(&TMP1);
    OUT(TMP1);
})

COM_DEF(in ,    0b00001000, {
    IN(&TMP1);
    PUSH(TMP1);
})

COM_DEF(jmp,    0b00110000, {
    JMP(ARG);
})

#define COM_DEF_JMP(name, value, mark)  \
    COM_DEF(name, value, {              \
        POP(&TMP1);                     \
        POP(&TMP2);                     \
        if(TMP1 mark TMP2){             \
            JMP(ARG);}                  \
    })                                  \

COM_DEF_JMP(ja , 0b00110001, > )

COM_DEF_JMP(jae, 0b00110010, >=)

COM_DEF_JMP(jb , 0b00110011, < )

COM_DEF_JMP(jbe, 0b00110100, <=)

COM_DEF_JMP(jeq, 0b00110101, ==)

COM_DEF_JMP(jne, 0b00110110, !=)

COM_DEF(call, 0b00111000, {
    PUSH(IP);
    JMP(ARG);
})

COM_DEF(ret, 0b00011001, {
    POP(&TMP1);
    JMP(TMP1);
})

#undef COM_DEF_JMP

