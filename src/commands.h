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
    PUSH(ARG);
})

COM_DEF(pop,    0b00000010, {
    POP(ARG);
})

COM_DEF(add,    0b00000011, {
    POP(TMP1);
    POP(TMP2);
    PUSH(TMP1 + TMP2);
})

COM_DEF(sub,    0b00000100, {
    POP(TMP1);
    POP(TMP2);
    PUSH(TMP1 - TMP2);
})

COM_DEF(mul,    0b00000101, {
    POP(TMP1);
    POP(TMP2);
    PUSH(TMP1 * TMP2);
})

COM_DEF(div,    0b00000110, {
    POP(TMP1);
    POP(TMP2);
    if(!TMP2){
        LOG_MESSAGE(ERROR, "Zero Division error");
        HLT;
    }
    PUSH(TMP1 / TMP2);
})

COM_DEF(out,    0b00000111, {
    POP(TMP1);
    OUT(TMP1);
})

COM_DEF(in ,    0b00001000, {
    PUSH(IN);
})