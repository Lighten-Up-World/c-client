#ifndef ENCODE_H
#define ENCODE_H

int encode(instruction_t *instr, word_t *word);
// int encodeCond(instruction_t *instr, word_t *word);
int encodeDP(instruction_t *instr, word_t *word);
int encodeMUL(instruction_t *instr, word_t *word);
int encodeSDT(instruction_t *instr, word_t *word);
int encodeBRN(instruction_t *instr, word_t *word);
#endif
