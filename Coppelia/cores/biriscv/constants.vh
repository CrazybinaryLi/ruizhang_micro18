//Register File
`define REG_SEL 5
//`define REG_NUM 2**`REG_SEL
`define REG_NUM 32

//`define the length of instruction
`define INSN_LEN 32

//Numbering each register
`define r0 `REG_SEL'd0
`define r1 `REG_SEL'd1
`define r2 `REG_SEL'd2
`define r3 `REG_SEL'd3
`define r4 `REG_SEL'd4
`define r5 `REG_SEL'd5
`define r6 `REG_SEL'd6
`define r7 `REG_SEL'd7
`define r8 `REG_SEL'd8
`define r9 `REG_SEL'd9
`define r10 `REG_SEL'd10
`define r11 `REG_SEL'd11
`define r12 `REG_SEL'd12
`define r13 `REG_SEL'd13
`define r14 `REG_SEL'd14
`define r15 `REG_SEL'd15
`define r16 `REG_SEL'd16
`define r17 `REG_SEL'd17
`define r18 `REG_SEL'd18
`define r19 `REG_SEL'd19
`define r20 `REG_SEL'd20
`define r21 `REG_SEL'd21
`define r22 `REG_SEL'd22
`define r23 `REG_SEL'd23
`define r24 `REG_SEL'd24
`define r25 `REG_SEL'd25
`define r26 `REG_SEL'd26
`define r27 `REG_SEL'd27
`define r28 `REG_SEL'd28
`define r29 `REG_SEL'd29
`define r30 `REG_SEL'd30
`define r31 `REG_SEL'd31

//-----------------------------------------------
//define RV32IM 
//-----------------------------------------------
//U type
`define OPCODE_LUI 7'b0110111
`define OPCODE_AUIPC 7'b0010111

//unconditioanl jump
`define OPCODE_JAL 7'b1101111
`define OPCODE_JALR 7'b1100111
`define FUNCT3_JALR 3'b000

//SB type:conditional jump
`define OPCODE_B 7'b1100011
`define FUNCT3_BEQ 3'b000
`define FUNCT3_BNE 3'b001
`define FUNCT3_BLT 3'b100
`define FUNCT3_BGE 3'b101
`define FUNCT3_BLTU 3'b110
`define FUNCT3_BGEU 3'b111

//I type:load operation
`define OPCODE_L 7'b0000011
`define FUNCT3_LB 3'b000
`define FUNCT3_LH 3'b001
`define FUNCT3_LW 3'b010
`define FUNCT3_LBU 3'b100
`define FUNCT3_LHU 3'b101

//S type:store operation
`define OPCODE_S 7'b0100011
`define FUNCT3_SB 3'b000
`define FUNCT3_SH 3'b001
`define FUNCT3_SW 3'b010

//I type
`define OPCODE_I 7'b0010011
`define FUNCT3_ADDI 3'b000
`define FUNCT3_SLTI 3'b010
`define FUNCT3_SLTIU 3'b011
`define FUNCT3_XORI 3'b100
`define FUNCT3_ORI 3'b110
`define FUNCT3_ANDI 3'b111
`define FUNCT3_SLLI 3'b001
`define FUNCT7_SLLI 7'b0000000
`define FUNCT3_SRLI 3'b101
`define FUNCT7_SRLI 7'b0000000
`define FUNCT3_SRAI 3'b101
`define FUNCT7_SRAI 7'b0100000

//R type
`define OPCODE_R 7'b0110011
`define FUNCT3_ADD 3'b000
`define FUNCT7_ADD 7'b0000000
`define FUNCT3_SUB 3'b000
`define FUNCT7_SUB 7'b0100000
`define FUNCT3_SLL 3'b001
`define FUNCT7_SLL 7'b0000000
`define FUNCT3_SLT 3'b010
`define FUNCT7_SLT 7'b0000000
`define FUNCT3_SLTU 3'b011
`define FUNCT7_SLTU 7'b0000000
`define FUNCT3_XOR 3'b100
`define FUNCT7_XOR 7'b0000000
`define FUNCT3_SRL 3'b101
`define FUNCT7_SRL 7'b0000000
`define FUNCT3_SRA 3'b101
`define FUNCT7_SRA 7'b0100000
`define FUNCT3_OR 3'b110
`define FUNCT7_OR 7'b0000000
`define FUNCT3_AND 3'b111
`define FUNCT7_AND 7'b0000000
//rv32M
`define FUNCT7_M 7'b0000001
`define FUNCT3_MUL 3'b000
`define FUNCT3_MULH 3'b001
`define FUNCT3_MULHSU 3'b010
`define FUNCT3_MULHU 3'b011
`define FUNCT3_DIV 3'b100
`define FUNCT3_DIVU 3'b101
`define FUNCT3_REM 3'b110
`define FUNCT3_REMU 3'b111