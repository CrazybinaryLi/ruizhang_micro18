module qed_property(
    input clk,
    input rst,
    input arfwe1,
    input arfwe2,
    input [`REG_SEL-1:0] dstarf1,
    input [`REG_SEL-1:0] dstarf2,
    input [`INSN_LEN-1:0] r1,
    input [`INSN_LEN-1:0] r2,
    input [`INSN_LEN-1:0] r3,
    input [`INSN_LEN-1:0] r4,
    input [`INSN_LEN-1:0] r5,
    input [`INSN_LEN-1:0] r6,
    input [`INSN_LEN-1:0] r7,
    input [`INSN_LEN-1:0] r8,
    input [`INSN_LEN-1:0] r9,
    input [`INSN_LEN-1:0] r10,
    input [`INSN_LEN-1:0] r11,
    input [`INSN_LEN-1:0] r12,
    input [`INSN_LEN-1:0] r13,
    input [`INSN_LEN-1:0] r14,
    input [`INSN_LEN-1:0] r15,
    input [`INSN_LEN-1:0] r16,
    input [`INSN_LEN-1:0] r17,
    input [`INSN_LEN-1:0] r18,
    input [`INSN_LEN-1:0] r19,
    input [`INSN_LEN-1:0] r20,
    input [`INSN_LEN-1:0] r21,
    input [`INSN_LEN-1:0] r22,
    input [`INSN_LEN-1:0] r23,
    input [`INSN_LEN-1:0] r24,
    input [`INSN_LEN-1:0] r25,
    input [`INSN_LEN-1:0] r26,
    input [`INSN_LEN-1:0] r27,
    input [`INSN_LEN-1:0] r28,
    input [`INSN_LEN-1:0] r29,
    input [`INSN_LEN-1:0] r30,
    input [`INSN_LEN-1:0] r31
);

    // EDIT: Insert the qed ready logic -- tracks number of committed instructions
    (* keep *) wire qed_ready;
    (* keep *) integer timecount = 0; 
    (* keep *) wire timeout; //If the specified time is exceeded, it will end.
    (* keep *) reg [15:0] num_orig_insts;
    (* keep *) reg [15:0] num_dup_insts;
    wire [1:0] num_orig_commits;
    wire [1:0] num_dup_commits;
    wire arfwe1,arfwe2;
    wire [4:0] dstarf1,dstarf2;

    // Instruction with destination register as 5'b0 is a NOP so ignore those
    assign num_orig_commits = ((arfwe1 == 1)&&(dstarf1 < 16)&&(dstarf1 != 5'b0)
			      &&(arfwe2 == 1)&&(dstarf2 < 16)&&(dstarf2 != 5'b0)) ? 2'b10 :
			     ((((arfwe1 == 1)&&(dstarf1 < 16)&&(dstarf1 != 5'b0)
			       &&((arfwe2 != 1)||(dstarf2 >= 16)||(dstarf2 == 5'b0)))
			      ||((arfwe2 == 1)&&(dstarf2 < 16)&&(dstarf2 != 5'b0)
				 &&((arfwe1 != 1)||(dstarf1 >= 16)||(dstarf1 == 5'b0)))) ? 2'b01 : 2'b00) ;

    // When destination register is 5'b0, it remains the same for both original and duplicate
    assign num_dup_commits = ((arfwe1 == 1)&&(dstarf1 >= 16)
			      &&(arfwe2 == 1)&&(dstarf2 >= 16)) ? 2'b10 :
			     ((((arfwe1 == 1)&&(dstarf1 >= 16)
			       &&((arfwe2 != 1)||(dstarf2 < 16)))
			      ||((arfwe2 == 1)&&(dstarf2 >= 16)
				 &&((arfwe1 != 1)||(dstarf1 < 16)))) ? 2'b01 : 2'b00) ;

    always @(posedge clk) begin
        if(rst) begin
            timecount <= 0;
        end 
        else begin
            timecount <= timecount + 1;
        end
    end

    always @(posedge clk) begin
	    if (rst) begin
	        num_orig_insts <= 16'b0;
	        num_dup_insts <= 16'b0;
	    end else begin
	        num_orig_insts <= num_orig_insts + {14'b0,num_orig_commits};
	        num_dup_insts <= num_dup_insts + {14'b0,num_dup_commits};
	    end
    end


    assign timeout = (timecount >= 50);
    assign qed_ready = (num_orig_insts == num_dup_insts);

    always @(posedge clk) begin
        if (qed_ready) begin
	        sqed: assert ( (r1 == r17) && (r2 == r18) && (r3 == r19) &&
                           (r4 == r20) && (r5 == r21) && (r6 == r22) &&
                           (r7 == r23) && (r8 == r24) && (r9 == r25) &&
                           (r10 == r26) && (r11 == r27) && (r12 == r28) &&
                           (r13 == r29) && (r14 == r30) && (r15 == r31)  
                         );
	    end
    end
    // EDIT: END

endmodule