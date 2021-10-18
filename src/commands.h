/**
 * We use marco COM_DEF(name, value, code)
 * name  - name of command
 * value - number of format 0b00000000
 *                            ^^^
 *                             ||
 * TODO: Documentation  
 */



#ifndef COM_DEF
#error  COM_DEF not defined
#endif

COM_DEF(hlt,    0b00000000, {
    
})

COM_DEF(push,   0b11100001, {

})

COM_DEF(pop,    0b00000010, {

})

COM_DEF(add,    0b00000011, {

})

COM_DEF(sub,    0b00000100, {

})

COM_DEF(mul,    0b00000101, {

})

COM_DEF(div,    0b00000110, {

})

COM_DEF(out,    0b00000111, {

})

COM_DEF(in ,    0b00001000, {

})