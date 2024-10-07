// Copyright (c) Institute of Software, Chinese Academy of Sciences
//
// This source code is patent protected and being made available under academic purpose.

// Author: Yufeng Li
// Email: crazybinary494@gmail.com

`include "constants.vh"

module inst_constraint(
                        // Inputs
                        input [`INSN_LEN-1:0] instruction,
                        input clk
                      );
  wire [4:0] shamt;
  wire [11:0] imm12;
  wire [4:0] rd;
  wire [2:0] funct3;
  wire [6:0] opcode;
  wire [6:0] imm7;
  wire [6:0] funct7;
  wire [4:0] imm5;
  wire [4:0] rs1;
  wire [4:0] rs2;
  wire [19:0] imm20;

  wire FORMAT_I;
  wire ALLOWED_I;
  wire ANDI;
  wire SLTIU;
  wire SRLI;
  wire SLTI;
  wire SRAI;
  wire SLLI;
  wire ORI;
  wire XORI;
  wire ADDI;
  wire LB;
  wire LH;
  wire LW;
  wire LBU;
  wire LHU;
  wire JALR;

  wire FORMAT_R;
  wire ALLOWED_R;
  wire AND;
  wire SLTU;
  wire MULH;
  wire SRA;
  wire XOR;
  wire SUB;
  wire SLT;
  wire MULHSU;
  wire MULHU;
  wire SRL;
  wire SLL;
  wire ADD;
  wire MUL;
  wire OR;
  wire DIV;
  wire DIVU;
  wire REM;
  wire REMU;

  wire FORMAT_S;
  wire ALLOWED_S;
  wire SW;
  wire SH;
  wire SB;

  wire FORMAT_SB;
  wire ALLOWED_SB;
  wire BEQ;
  wire BNE;
  wire BLT;
  wire BGE;
  wire BLTU;
  wire BGEU;

  wire FORMAT_U;
  wire ALLOWED_U;
  wire LUI;
  wire AUIPC;

  wire FORMAT_UJ;
  wire JAL;
  wire ALLOWED_UJ;

  wire ALLOWED_NOP;
  wire NOP;

  assign shamt = instruction[24:20];
  assign imm12 = instruction[31:20];
  assign rd = instruction[11:7];
  assign funct3 = instruction[14:12];
  assign opcode = instruction[6:0];
  assign imm7 = instruction[31:25];
  assign funct7 = instruction[31:25];
  assign imm5 = instruction[11:7];
  assign rs1 = instruction[19:15];
  assign rs2 = instruction[24:20];
  assign imm20 = instruction[31:12];

  assign FORMAT_I = (rs1 < 16) && (rd < 16) && (rd != 0);
  assign ANDI = FORMAT_I && (funct3 == 3'b111) && (opcode == 7'b0010011);
  assign SLTIU = FORMAT_I && (funct3 == 3'b011) && (opcode == 7'b0010011);
  assign SRLI = FORMAT_I && (funct3 == 3'b101) && (opcode == 7'b0010011) && (funct7 == 7'b0000000);
  assign SLTI = FORMAT_I && (funct3 == 3'b010) && (opcode == 7'b0010011);
  assign SRAI = FORMAT_I && (funct3 == 3'b101) && (opcode == 7'b0010011) && (funct7 == 7'b0100000);
  assign SLLI = FORMAT_I && (funct3 == 3'b001) && (opcode == 7'b0010011) && (funct7 == 7'b0000000);
  assign ORI = FORMAT_I && (funct3 == 3'b110) && (opcode == 7'b0010011);
  assign XORI = FORMAT_I && (funct3 == 3'b100) && (opcode == 7'b0010011);
  assign ADDI = FORMAT_I && (funct3 == 3'b000) && (opcode == 7'b0010011);
  assign LW = FORMAT_I && (funct3 == 3'b010) && (opcode == 7'b0000011) && (rs1 == 5'b00000) && (imm12[11]==1'b1);
  assign LB = FORMAT_I && (funct3 == 3'b000) && (opcode == 7'b0000011) && (rs1 == 5'b00000) && (imm12[11]==1'b1);
  assign LH = FORMAT_I && (funct3 == 3'b001) && (opcode == 7'b0000011) && (rs1 == 5'b00000) && (imm12[11]==1'b1);
  assign LBU = FORMAT_I && (funct3 == 3'b100) && (opcode == 7'b0000011) && (rs1 == 5'b00000) && (imm12[11]==1'b1);
  assign LHU = FORMAT_I && (funct3 == 3'b101) && (opcode == 7'b0000011) && (rs1 == 5'b00000);
  assign JALR = FORMAT_I && (funct3 == 3'b000) && (opcode == 7'b1100111);
  assign ALLOWED_I = ANDI || SLTIU || SRLI || SLTI || SRAI || SLLI || ORI || XORI || ADDI || LB || LH || LW || LBU || LHU;

  assign FORMAT_R = (rs2 < 16) && (rs1 < 16) && (rd < 16) && (rd != 0);
  assign AND = FORMAT_R && (funct3 == 3'b111) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign SLTU = FORMAT_R && (funct3 == 3'b011) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign MULH = FORMAT_R && (funct3 == 3'b001) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign SRA = FORMAT_R && (funct3 == 3'b101) && (opcode == 7'b0110011) && (funct7 == 7'b0100000);
  assign XOR = FORMAT_R && (funct3 == 3'b100) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign SUB = FORMAT_R && (funct3 == 3'b000) && (opcode == 7'b0110011) && (funct7 == 7'b0100000);
  assign SLT = FORMAT_R && (funct3 == 3'b010) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign MULHSU = FORMAT_R && (funct3 == 3'b010) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign MULHU = FORMAT_R && (funct3 == 3'b011) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign SRL = FORMAT_R && (funct3 == 3'b101) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign SLL = FORMAT_R && (funct3 == 3'b001) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign ADD = FORMAT_R && (funct3 == 3'b000) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign MUL = FORMAT_R && (funct3 == 3'b000) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign OR = FORMAT_R && (funct3 == 3'b110) && (opcode == 7'b0110011) && (funct7 == 7'b0000000);
  assign DIV = FORMAT_R && (funct3 == 3'b100) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign DIVU = FORMAT_R && (funct3 == 3'b101) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign REM = FORMAT_R && (funct3 == 3'b110) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign REMU = FORMAT_R && (funct3 == 3'b111) && (opcode == 7'b0110011) && (funct7 == 7'b0000001);
  assign ALLOWED_R = AND || SLTU || MULH || SRA || XOR || SUB || SLT || MULHSU || MULHU || SRL || SLL || ADD || MUL || OR || DIV || DIVU || REM || REMU;

  assign FORMAT_S = (rs2 < 16) && (rs1 < 16);
  assign SW = FORMAT_S && (funct3 == 3'b010) && (opcode == 7'b0100011) && (rs1 == 5'b00000) && (imm7[6]==1'b1);
  assign SH = FORMAT_S && (funct3 == 3'b001) && (opcode == 7'b0100011) && (rs1 == 5'b00000) && (imm7[6]==1'b1);
  assign SB = FORMAT_S && (funct3 == 3'b000) && (opcode == 7'b0100011) && (rs1 == 5'b00000) && (imm7[6]==1'b1);
  assign ALLOWED_S = SW || SH || SB;
  
  assign FORMAT_SB = (rs2 < 16) && (rs1 < 16);
  assign BEQ = FORMAT_SB && (funct3 == 3'b000) && (opcode == 7'b1100011);
  assign BNE = FORMAT_SB && (funct3 == 3'b001) && (opcode == 7'b1100011);
  assign BLT = FORMAT_SB && (funct3 == 3'b100) && (opcode == 7'b1100011);
  assign BGE = FORMAT_SB && (funct3 == 3'b101) && (opcode == 7'b1100011);
  assign BLTU = FORMAT_SB && (funct3 == 3'b110) && (opcode == 7'b1100011);
  assign BGEU = FORMAT_SB && (funct3 == 3'b111) && (opcode == 7'b1100011);
  assign ALLOWED_SB = BEQ || BNE || BLT || BGE || BLTU || BGEU;

  assign FORMAT_U = (rd < 16) && (rd != 0);
  assign LUI = FORMAT_U && (opcode == 7'b0110111);
  assign AUIPC = FORMAT_U && (opcode == 7'b0010111);//PC is non-consistent, so AUIPC can't be tested.
  assign ALLOWED_U = LUI;

  assign FORMAT_UJ = (rd < 16) && (rd != 0); 
  assign JAL = FORMAT_UJ && (opcode == 7'b1101111);
  assign ALLOWED_UJ = JAL;

  assign NOP = (imm12 == 12'b0) && (rs1 == 0) && (funct3 == 3'b000) && (rd == 0) && (opcode == 7'b0010011);
  assign ALLOWED_NOP = NOP;


  `ifndef VERILATOR
  always @(posedge clk) begin
    assume property (ALLOWED_I || ALLOWED_U || ALLOWED_R || ALLOWED_S || ALLOWED_NOP);
  end
  `endif

endmodule