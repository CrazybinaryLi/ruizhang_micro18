#ifndef __RIDECORE_CONSTRAINTS_H_
#define __RIDECORE_CONSTRAINTS_H_

#include "stdint.h"
#include "stdio.h"

#define BIT_RANGE(instr, upper, lower) \
  (instr & (((1 << (upper - lower + 1)) - 1) << lower))
#define BIT_SINGLE(instr, pos) (instr & (1 << pos))

#define instr_csr(instr) (BIT_RANGE((uint32_t)instr, 31, 20) >> 20)
#define instr_zimm(instr) (BIT_RANGE(instr, 19, 15) >> 15)
#define instr_shamt(instr) (BIT_RANGE(instr, 24, 20) >> 20)
#define instr_funct3(instr) (BIT_RANGE(instr, 14, 12) >> 12)
#define instr_funct12(instr) (BIT_RANGE((uint32_t)instr, 31, 20) >> 20)
#define instr_funct7(instr) (BIT_RANGE((uint32_t)instr, 31, 25) >> 25)
#define instr_funct5(instr) (BIT_RANGE((uint32_t)instr, 31, 27) >> 27)
#define instr_aq(instr) BIT_SINGLE(instr, 26)
#define instr_rl(instr) BIT_SINGLE(instr, 25)
#define instr_opcode(instr) BIT_RANGE(instr, 6, 0)

int32_t instr_J_imm(int32_t instr) {
  return (BIT_SINGLE(instr, 31) >> 11) | BIT_RANGE(instr, 19, 12) |
         (BIT_SINGLE(instr, 20) >> 9) | (BIT_RANGE(instr, 30, 21) >> 20);
}

int32_t instr_B_imm(int32_t instr) {
  return ((BIT_SINGLE(instr, 31) >> 19) | (BIT_SINGLE(instr, 7) << 4) |
          (BIT_RANGE(instr, 30, 25) >> 20) | (BIT_RANGE(instr, 11, 8) >> 7));
}

#define instr_I_imm(instr) (((int32_t)BIT_RANGE(instr, 31, 20)) >> 20)
#define instr_S_imm(instr) \
  (BIT_RANGE(instr, 31, 25) >> 20) | (BIT_RANGE(instr, 11, 7) >> 7)
#define instr_U_imm(instr) BIT_RANGE(instr, 31, 12)
#define instr_rs1(instr) (BIT_RANGE(instr, 19, 15) >> 15)
#define instr_rs2(instr) (BIT_RANGE(instr, 24, 20) >> 20)
#define instr_rd(instr) (BIT_RANGE(instr, 11, 7) >> 7)

// RV32 opcode
// RV32I Base Instruction Set
#define LUI_OP 0b0110111
#define AUIPC_OP 0b0010111
#define JAL_OP 0b1101111
#define JALR_OP 0b1100111
#define BEQ_OP 0b1100011
#define BNE_OP 0b1100011
#define BLT_OP 0b1100011
#define BGE_OP 0b1100011
#define BLTU_OP 0b1100011
#define BGEU_OP 0b1100011
#define LB_OP 0b0000011
#define LH_OP 0b0000011
#define LW_OP 0b0000011
#define LBU_OP 0b0000011
#define LHU_OP 0b0000011
#define SB_OP 0b0100011
#define SH_OP 0b0100011
#define SW_OP 0b0100011
#define ADDI_OP 0b0010011
#define SLTI_OP 0b0010011
#define SLTIU_OP 0b0010011
#define XORI_OP 0b0010011
#define ORI_OP 0b0010011
#define ANDI_OP 0b0010011
#define SLLI_OP 0b0010011
#define SRLI_OP 0b0010011
#define SRAI_OP 0b0010011
#define ADD_OP 0b0110011
#define SUB_OP 0b0110011
#define SLL_OP 0b0110011
#define SLT_OP 0b0110011
#define SLTU_OP 0b0110011
#define XOR_OP 0b0110011
#define SRL_OP 0b0110011
#define SRA_OP 0b0110011
#define OR_OP 0b0110011
#define AND_OP 0b0110011
#define FENCE_OP 0b0001111
#define ECALL_OP 0b1110011
#define EBREAK_OP 0b1110011
#define CS_OP 0b1110011

// RV32M Standard Extension
#define MUL_OP 0b0110011
#define MULH_OP 0b0110011
#define MULHSU_OP 0b0110011
#define MULHU_OP 0b0110011
#define DIV_OP 0b0110011
#define DIVU_OP 0b0110011
#define REM_OP 0b0110011
#define REMU_OP 0b0110011

// RV32A Standard Extension
#define LR_W_OP 0b0101111
#define SC_W_OP 0b0101111
#define AMOSWAP_W_OP 0b0101111
#define AMOADD_W_OP 0b0101111
#define AMOXOR_W_OP 0b0101111
#define AMOAND_W_OP 0b0101111
#define AMOOR_W_OP 0b0101111
#define AMOMIN_W_OP 0b0101111
#define AMOMAX_W_OP 0b0101111
#define AMOMINU_W_OP 0b0101111
#define AMOMAXU_W_OP 0b0101111

// privileged instructions
#define URET_OP 0b1110011
#define SRET_OP 0b1110011
#define MRET_OP 0b1110011
#define WFI_OP 0b1110011
#define SFENCE_VMA_OP 0b1110011

#define RV32IM_R 0b0110011
#define RV32I_B 0b1100011
#define RV32I_S 0b0100011
#define RV32I_L 0b0000011
#define RV32I_I 0b0010011

// RV32 funct3 field
#define FUNCT3_JALR 0b000
#define FUNCT3_BEQ 0b000
#define FUNCT3_BNE 0b001
#define FUNCT3_BLT 0b100
#define FUNCT3_BGE 0b101
#define FUNCT3_BLTU 0b110
#define FUNCT3_BGEU 0b111
#define FUNCT3_LB 0b000
#define FUNCT3_LH 0b001
#define FUNCT3_LW 0b010
#define FUNCT3_LBU 0b100
#define FUNCT3_LHU 0b101
#define FUNCT3_SB 0b000
#define FUNCT3_SH 0b001
#define FUNCT3_SW 0b010
#define FUNCT3_ADDI 0b000
#define FUNCT3_SLTI 0b010
#define FUNCT3_SLTIU 0b011
#define FUNCT3_XORI 0b100
#define FUNCT3_ORI 0b110
#define FUNCT3_ANDI 0b111
#define FUNCT3_SLLI 0b001
#define FUNCT3_SRLI 0b101
#define FUNCT3_SRAI 0b101
#define FUNCT3_ADD 0b000
#define FUNCT3_SUB 0b000
#define FUNCT3_SLL 0b001
#define FUNCT3_SLT 0b010
#define FUNCT3_SLTU 0b011
#define FUNCT3_XOR 0b100
#define FUNCT3_SRL 0b101
#define FUNCT3_SRA 0b101
#define FUNCT3_OR 0b110
#define FUNCT3_AND 0b111
#define FUNCT3_FENCE 0b000
#define FUNCT3_FENCE_I 0b001
#define FUNCT3_ECALL 0b000
#define FUNCT3_EBREAK 0b000
#define FUNCT3_CSRRW 0b001
#define FUNCT3_CSRRS 0b010
#define FUNCT3_CSRRC 0b011
#define FUNCT3_CSRRWI 0b101
#define FUNCT3_CSSRRSI 0b110
#define FUNCT3_CSRRCI 0b111
#define FUNCT3_MUL 0b000
#define FUNCT3_MULH 0b001
#define FUNCT3_MULHSU 0b010
#define FUNCT3_MULHU 0b011
#define FUNCT3_DIV 0b100
#define FUNCT3_DIVU 0b101
#define FUNCT3_REM 0b110
#define FUNCT3_REMU 0b111

// RV32 funct7 field
#define FUNCT7_SLLI 0b0000000
#define FUNCT7_SRLI 0b0000000
#define FUNCT7_SRAI 0b0100000
#define FUNCT7_ADD 0b0000000
#define FUNCT7_SUB 0b0100000
#define FUNCT7_SLL 0b0000000
#define FUNCT7_SLT 0b0000000
#define FUNCT7_SLTU 0b0000000
#define FUNCT7_XOR 0b0000000
#define FUNCT7_SRL 0b0000000
#define FUNCT7_SRA 0b0100000
#define FUNCT7_OR 0b0000000
#define FUNCT7_AND 0b0000000
#define FUNCT7_M 0b0000001

uint8_t rs1_is_original(uint32_t instr) {
  return ((uint8_t)instr_rs1(instr) < 16);
}

uint8_t rs2_is_original(uint32_t instr) {
  return ((uint8_t)instr_rs2(instr) < 16);
}

uint8_t rd_is_original(uint32_t instr) {
  return ((uint8_t)instr_rd(instr) < 16);
}


void rv_constraints(uint32_t instr) {
  klee_assume(
      /*
      // U-type LUI
      (instr_opcode(instr) == LUI_OP) |
      // U-type AUIPC
      (instr_opcode(instr) == AUIPC_OP) |
      // J-type JAL
      (instr_opcode(instr) == JAL_OP) |
      // J-type JALR
      (instr_opcode(instr) == JALR_OP & instr_funct3(instr) == FUNCT3_JALR) |
      // B-type BEQ
      (instr_opcode(instr) == RV32I_B & instr_funct3(instr) == FUNCT3_BEQ) |
      // B-type BNE
      (instr_opcode(instr) == RV32I_B & instr_funct3(instr) == FUNCT3_BNE) |
      // B-type BLT
      (instr_opcode(instr) == RV32I_B & instr_funct3(instr) == FUNCT3_BLT) |
      // B-type BGE
      (instr_opcode(instr) == RV32I_B & instr_funct3(instr) == FUNCT3_BGE) |
      // B-type BLTU
      (instr_opcode(instr) == RV32I_B & instr_funct3(instr) == FUNCT3_BLTU) |
      // B-type BGEU
      (instr_opcode(instr) == RV32I_B & instr_funct3(instr) == FUNCT3_BGEU) |
      // I-type LB
      (instr_opcode(instr) == RV32I_L & instr_funct3(instr) == FUNCT3_LB) |
      // I-type LH
      (instr_opcode(instr) == RV32I_L & instr_funct3(instr) == FUNCT3_LH) |
      */
      // I-type LW
      (BIT_RANGE(instr, 31, 30) == 0b00 & instr_opcode(instr) == RV32I_L & instr_funct3(instr) == FUNCT3_LW & rs1_is_original(instr) & rd_is_original(instr)) |
      /*
      // I-type LBU
      (instr_opcode(instr) == RV32I_L & instr_funct3(instr) == FUNCT3_LBU) |
      // I-type LHU
      (instr_opcode(instr) == RV32I_L & instr_funct3(instr) == FUNCT3_LHU) |
      // S-type SB
      (instr_opcode(instr) == RV32I_S & instr_funct3(instr) == FUNCT3_SB) |
      // S-type SH
      (instr_opcode(instr) == RV32I_S & instr_funct3(instr) == FUNCT3_SH) |
      */
      // S-type SW
      (BIT_RANGE(instr, 31, 30) == 0b00 & instr_opcode(instr) == RV32I_S & instr_funct3(instr) == FUNCT3_SW & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type ADDI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_ADDI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type SLTI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_SLTI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type SLTIU
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_SLTIU & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type XORI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_XORI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type ORI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_ORI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type ANDI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_ANDI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type SLLI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_SLLI &
       instr_funct7(instr) == FUNCT7_SLLI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type SRLI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_SRLI &
       instr_funct7(instr) == FUNCT7_SRLI & rs1_is_original(instr) & rd_is_original(instr)) |
      // I-type SRAI
      (instr_opcode(instr) == RV32I_I & instr_funct3(instr) == FUNCT3_SRAI &
       instr_funct7(instr) == FUNCT7_SRAI & rs1_is_original(instr) & rd_is_original(instr)) |
      // R-type ADD
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_ADD &
       instr_funct7(instr) == FUNCT7_ADD & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type SUB
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_SUB &
       instr_funct7(instr) == FUNCT7_SUB & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type SLL
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_SLL &
       instr_funct7(instr) == FUNCT7_SLL & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type SLT
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_SLT &
       instr_funct7(instr) == FUNCT7_SLT & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type SLTU
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_SLTU &
       instr_funct7(instr) == FUNCT7_SLTU & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type XOR
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_XOR &
       instr_funct7(instr) == FUNCT7_XOR & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type SRL
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_SRL &
       instr_funct7(instr) == FUNCT7_SRL & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type SRA
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_SRA &
       instr_funct7(instr) == FUNCT7_SRA & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type OR
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_OR &
       instr_funct7(instr) == FUNCT7_OR & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // R-type AND
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_AND &
       instr_funct7(instr) == FUNCT7_AND & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      /*// I-type FENCE
      (instr_opcode(instr) == FENCE_OP & instr_rd(instr) == 0b00000 &
       instr_funct3(instr) == FUNCT3_FENCE & instr_rs1(instr) == 0b00000 &
       BIT_RANGE(instr, 31, 28) == 0b0000) |
      // I-type FENCE.I
      (instr_opcode(instr) == FENCE_OP & instr_rd(instr) == 0b00000 &
       instr_funct3(instr) == FUNCT3_FENCE_I & instr_rs1(instr) == 0b00000 &
       BIT_RANGE(instr, 31, 20) == 0b000000000000) |
      // I-type ECALL
      (instr_opcode(instr) == ECALL_OP & instr_rd(instr) == 0b00000 &
       instr_funct3(instr) == FUNCT3_ECALL & instr_rs1(instr) == 0b00000 &
       BIT_RANGE(instr, 31, 20) == 0b000000000000) |
      // I-type EBREAK
      (instr_opcode(instr) == EBREAK_OP & instr_rd(instr) == 0b00000 &
       instr_funct3(instr) == FUNCT3_EBREAK instr_rs1(instr) == 0b00000 &
       BIT_RANGE(instr, 31, 20) == 0b000000000001) |
      // I-type CSRRW
      (instr_opcode(instr) == CS_OP & instr_funct3(instr) == FUNCT3_CSRRW) |
      // I-type CSRRS
      (instr_opcode(instr) == CS_OP & instr_funct3(instr) == FUNCT3_CSRRS) |
      // I-type CSRRC
      (instr_opcode(instr) == CS_OP & instr_funct3(instr) == FUNCT3_CSRRC) |
      // I-type CSRRWI
      (instr_opcode(instr) == CS_OP & instr_funct3(instr) == FUNCT3_CSRRWI) |
      // I-type CSSRRSI
      (instr_opcode(instr) == CS_OP & instr_funct3(instr) == FUNCT3_CSSRRSI) |
      // I-type CSRRCI
      (instr_opcode(instr) == CS_OP & instr_funct3(instr) == FUNCT3_CSRRCI) |
      */
      // MUL
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MUL &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // MULH
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MULH &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // MULHSU
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MULHSU &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // MULHU
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MULHU &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      /*
      // DIV
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_DIV &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // DIVU
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_DIVU &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // REM
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_REM &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      // REMU
      (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_REMU &
       instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
      */
      (instr_opcode(instr) == 0b1111111)
      );
  return;
}

void rv_constraints_partial(uint32_t instr) {
  klee_assume(
    // MUL
    // (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MUL &
    // instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
    // MULH
    (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MULH &
    instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
    // MULHSU
    (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MULHSU &
    instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) |
    // MULHU
    (instr_opcode(instr) == RV32IM_R & instr_funct3(instr) == FUNCT3_MULHU & 
      instr_funct7(instr) == FUNCT7_M & rs1_is_original(instr) & rs2_is_original(instr) & rd_is_original(instr)) 
  );
}

#endif /* __RIDECORE_CONSTRAINTS_H_ */
