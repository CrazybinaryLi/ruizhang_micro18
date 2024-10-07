// Copyright (c) Stanford University
// 
// This source code is patent protected and being made available under the
// terms explained in the ../LICENSE-Academic and ../LICENSE-GOV files.
//
// Author: Mario J Srouji
// Email: msrouji@stanford.edu

module qed_decoder (
// Outputs
shamt,
IS_S,
imm12,
IS_R,
rd,
funct3,
opcode,
rs2,
funct7,
IS_I,
imm5,
rs1,
imm7,
imm20,
IS_SB,
IS_U,
IS_UJ,
// Inputs
ifu_qed_instruction);

  input [31:0] ifu_qed_instruction;

  output [4:0] shamt;
  output IS_S;
  output [11:0] imm12;
  output IS_R;
  output [4:0] rd;
  output [2:0] funct3;
  output [6:0] opcode;
  output [4:0] rs2;
  output [6:0] funct7;
  output IS_I;
  output IS_SB;
  output IS_U;
  output IS_UJ;
  output [4:0] imm5;
  output [4:0] rs1;
  output [6:0] imm7;
  output [19:0] imm20;

  assign shamt = ifu_qed_instruction[24:20];
  assign imm12 = ifu_qed_instruction[31:20];
  assign rd = ifu_qed_instruction[11:7];
  assign funct3 = ifu_qed_instruction[14:12];
  assign opcode = ifu_qed_instruction[6:0];
  assign imm7 = ifu_qed_instruction[31:25];
  assign funct7 = ifu_qed_instruction[31:25];
  assign imm5 = ifu_qed_instruction[11:7];
  assign rs1 = ifu_qed_instruction[19:15];
  assign rs2 = ifu_qed_instruction[24:20];
  assign imm20 = ifu_qed_instruction[31:12];

  assign IS_I = (opcode == 7'b0010011) || (opcode == 7'b0000011) || (opcode == 7'b1100111);
  assign IS_R = (opcode == 7'b0110011);
  assign IS_S = (opcode == 7'b0100011);
  assign IS_SB = (opcode == 7'b1100011);
  assign IS_U = (opcode == 7'b0110111) || (opcode == 7'b0010111);
  assign IS_UJ = (opcode == 7'b1101111);

endmodule