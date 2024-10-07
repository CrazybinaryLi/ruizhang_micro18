// Copyright (c) Stanford University
// 
// This source code is patent protected and being made available under the
// terms explained in the ../LICENSE-Academic and ../LICENSE-GOV files.
//
// Author: Mario J Srouji
// Email: msrouji@stanford.edu

module modify_instruction (
// Outputs
qed_instruction,
// Inputs
shamt,
IS_S,
imm12,
IS_R,
qic_qimux_instruction,
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
IS_UJ);

  input [4:0] shamt;
  input IS_S;
  input [11:0] imm12;
  input IS_R;
  input [31:0] qic_qimux_instruction;
  input [4:0] rd;
  input [2:0] funct3;
  input [6:0] opcode;
  input [4:0] rs2;
  input [6:0] funct7;
  input IS_I;
  input [4:0] imm5;
  input [4:0] rs1;
  input [6:0] imm7;
  input [19:0] imm20;
  input IS_SB;
  input IS_U;
  input IS_UJ;


  output [31:0] qed_instruction;

  wire [31:0] INS_I;
  wire [31:0] INS_R;
  wire [31:0] INS_S;
  wire [31:0] INS_SB;
  wire [31:0] INS_U;
  wire [31:0] INS_UJ;

  wire [4:0] NEW_rd;
  wire [4:0] NEW_rs1;
  wire [4:0] NEW_rs2;
  wire [11:0] NEW_imm12;
  wire [4:0] NEW_imm5;
  wire [6:0] NEW_imm7;
  wire [19:0] NEW_imm20;

  assign NEW_rd = (rd == 5'b00000) ? rd : {1'b1, rd[3:0]};
  assign NEW_rs1 = (rs1 == 5'b00000) ? rs1 : {1'b1, rs1[3:0]};
  assign NEW_rs2 = (rs2 == 5'b00000) ? rs2 : {1'b1, rs2[3:0]};
  assign NEW_imm12 = imm12 + 12'b100000000000;
  assign NEW_imm5 = imm5 + 5'b10000;
  assign NEW_imm7 = imm7 + 7'b1000000;
  assign NEW_imm20 = imm20 + 20'b10000000000000000000;

  assign INS_I = (opcode == 7'b0000011) ? {NEW_imm12, NEW_rs1, funct3, NEW_rd, opcode}: {imm12, NEW_rs1, funct3, NEW_rd, opcode};
  assign INS_R = {funct7, NEW_rs2, NEW_rs1, funct3, NEW_rd, opcode};
  assign INS_S = {NEW_imm7, NEW_rs2, NEW_rs1, funct3, NEW_imm5, opcode};
  assign INS_SB = {imm7, NEW_rs2, NEW_rs1, funct3, imm5, opcode};
  assign INS_U = {imm20, NEW_rd, opcode};
  assign INS_UJ = {imm20, NEW_rd, opcode};

  assign qed_instruction = IS_I ? INS_I : (IS_R ? INS_R : (IS_S ? INS_S : (IS_SB ? INS_SB : (IS_U ? INS_U : (IS_UJ ? INS_UJ : qic_qimux_instruction)))));

endmodule