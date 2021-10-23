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
 * Command list (with mask 0b00011111):
 * 0x00 - htl
 * 0x01 - push
 * 0x02 - pop
 * 0x03 - add
 * 0x04 - sub
 * 0x05 - mul
 * 0x06 - div
 * 0x07 - out
 * 0x08 - in
 * 0x09 - outc
 * 0x0A - 
 * 0x0B - 
 * 0x0C -
 * 0x0D - sleep
 * 0x0E - pause
 * 0x0F - 
 * 0x10 - jmp
 * 0x11 - ja
 * 0x12 - jae
 * 0x13 - jb
 * 0x14 - jbe
 * 0x15 - jeq
 * 0x16 - jne
 * 0x17 - 
 * 0x18 - call
 * 0x19 - ret
 * 0x1A - 
 * 0x1B - 
 * 0x1C - 
 * 0x1D -
 * 0x1E - 
 * 0x1F - ERROR
 
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

COM_DEF(outc,   0b00001001, {
    POP(&TMP1);
    OUTC(TMP1);
})

COM_DEF(pau,  0b00001101, {
    PAUSE;
})

COM_DEF(slp,  0b00001110, {
    POP(&TMP1)
    SLEEP(TMP1);
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

