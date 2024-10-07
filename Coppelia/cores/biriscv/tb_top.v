module tb_top(
    //EDIT
    input clk,
    input rst,
    input qed_exec_dup, // comment when set up model checking

    input [31:0] instruction
    //EDIT: END
);

// EDIT
// reg clk;
// reg rst;

// reg [7:0] mem[131071:0];//128KB ram
// integer i;input [31:0] instruction
// integer f;

// initial
// begin
//     $display("Starting bench");

//     if (`TRACE)
//     begin
//         $dumpfile("waveform.vcd");
//         $dumpvars(0, tbr1_top);
//     end

//     // Reset
//     clk = 0;
//     rst = 1;
//     repeat (5) @(posedge clk);
//     rst = 0;

//     // Load TCM memory
//     for (i=0;i<131072;i=i+1)
//         mem[i] = 0;

//     f = $fopenr("./build/tcm.bin");
//     i = $fread(mem, f);
//     for (i=0;i<131072;i=i+1)
//         u_mem.write(i, mem[i]); //write data to tcm,each line 64bit.
// end

// initial
// begin
//     forever
//     begin 
//         clk = #5 ~clk;
//     endmispredicted_r
// end
// EDIT: END

`ifndef VERILATOR
// EDIT
// embed the assumption that there's no reset
// into the generated BTOR2
// we will run the reset sequence in Yosys before generating the BTOR2
// it will complain about the violated assumption, but should work fine otherwise
// this is a pos-edge reset, so we assume it is 0.
always @(*) begin
    no_reset: assume(~rst);
//EDIT: END
`endif

//EDIT: Use the inst_constraint module to constrain instruction to be a valid instruction from the ISA
inst_constraint inst_constraint0(.clk(clk),
                                 .instruction(instruction));
//EDIT: END

wire          mem_i_rd_w;
wire          mem_i_flush_w;
wire          mem_i_invalidate_w;
wire [ 31:0]  mem_i_pc_w;
wire [ 31:0]  mem_d_addr_w;
wire [ 31:0]  mem_d_data_wr_w;
wire          mem_d_rd_w;
wire [  3:0]  mem_d_wr_w;
wire          mem_d_cacheable_w;
wire [ 10:0]  mem_d_req_tag_w;
wire          mem_d_invalidate_w;
wire          mem_d_writeback_w;
wire          mem_d_flush_w;
wire          mem_i_accept_w;
wire          mem_i_valid_w;
wire          mem_i_error_w;
wire [ 63:0]  mem_i_inst_w;
wire [ 31:0]  mem_d_data_rd_w;
wire          mem_d_accept_w;
wire          mem_d_ack_w;
wire          mem_d_error_w;
wire [ 10:0]  mem_d_resp_tag_w;

// EDIT: add in the QED module.
wire qed_vld_out;
wire [31:0] qed_ifu_instruction;
wire [63:0] sch_ifu_instruction;
// wire qed_exec_dup; // uncomment when set up model checking
reg sch_i_rd_q;

always @(posedge clk) begin
    if(rst) sch_i_rd_q <= 1'b0;
    else sch_i_rd_q <= mem_i_rd_w;
end

// instruction and qed_exec_dup are cutpoints
qed qed0 ( // Inputs
        .clk(clk),
        .rst(rst),
        .ena(1'b1),
        .ifu_qed_instruction(instruction),
        .exec_dup(qed_exec_dup),
        .stall_IF(~sch_i_rd_q),
        // Outputs
        .qed_ifu_instruction(qed_ifu_instruction),  
        .vld_out(qed_vld_out)); //valid signal

assign sch_ifu_instruction = {32'h00000013,qed_ifu_instruction};

// EDIT: END

riscv_core u_dut
//-----------------------------------------------------------------
// Ports
//-----------------------------------------------------------------
(
    // Inputs
     .clk_i(clk)
    ,.rst_i(rst)
    ,.mem_d_data_rd_i(mem_d_data_rd_w) //input data
    ,.mem_d_accept_i(mem_d_accept_w)   //data accept signal
    ,.mem_d_ack_i(mem_d_ack_w)
    ,.mem_d_error_i(mem_d_error_w)
    ,.mem_d_resp_tag_i(mem_d_resp_tag_w) //data response signal tcm->cpu
    ,.mem_i_accept_i(mem_i_accept_w)
    //EDIT
    //,.mem_i_valid_i(mem_i_valid_w)
    ,.mem_i_valid_i(qed_vld_out)
    ,.mem_i_error_i(mem_i_error_w)
    //,.mem_i_inst_i(mem_i_inst_w)       //instruction 64bit
    ,.mem_i_inst_i(sch_ifu_instruction)
    //EDIT: END
    ,.intr_i(1'b0)
    ,.reset_vector_i(32'h80000000)
    ,.cpu_id_i('b0)

    // Outputs
    ,.mem_d_addr_o(mem_d_addr_w)   //request data address
    ,.mem_d_data_wr_o(mem_d_data_wr_w) //output 32bit data
    ,.mem_d_rd_o(mem_d_rd_w) //read enable
    ,.mem_d_wr_o(mem_d_wr_w) //write enable
    ,.mem_d_cacheable_o(mem_d_cacheable_w)
    ,.mem_d_req_tag_o(mem_d_req_tag_w) //data request tag
    ,.mem_d_invalidate_o(mem_d_invalidate_w) //data invalide signal
    ,.mem_d_writeback_o(mem_d_writeback_w)   //data writeback signal
    ,.mem_d_flush_o(mem_d_flush_w)
    ,.mem_i_rd_o(mem_i_rd_w) //fetch instruction signal 
    ,.mem_i_flush_o(mem_i_flush_w) 
    ,.mem_i_invalidate_o(mem_i_invalidate_w)
    ,.mem_i_pc_o(mem_i_pc_w) //instruction address
);

tcm_mem u_mem
(
    // Inputs
     .clk_i(clk)
    ,.rst_i(rst)
    ,.mem_i_rd_i(mem_i_rd_w) //fetch instruction signal
    ,.mem_i_flush_i(mem_i_flush_w) 
    ,.mem_i_invalidate_i(mem_i_invalidate_w)
    ,.mem_i_pc_i(mem_i_pc_w) //instruction address
    ,.mem_d_addr_i(mem_d_addr_w) //data address
    ,.mem_d_data_wr_i(mem_d_data_wr_w) //data saves to tcm
    ,.mem_d_rd_i(mem_d_rd_w) 
    ,.mem_d_wr_i(mem_d_wr_w) //firt part data or second part data 
    ,.mem_d_cacheable_i(mem_d_cacheable_w)
    ,.mem_d_req_tag_i(mem_d_req_tag_w) //data response signal cpu->tcm
    ,.mem_d_invalidate_i(mem_d_invalidate_w)
    ,.mem_d_writeback_i(mem_d_writeback_w)
    ,.mem_d_flush_i(mem_d_flush_w)

    // Outputs
    ,.mem_i_accept_o(mem_i_accept_w)
    ,.mem_i_valid_o(mem_i_valid_w)     //instruction valid signal
    ,.mem_i_error_o(mem_i_error_w)
    ,.mem_i_inst_o(mem_i_inst_w)       //output instruction
    ,.mem_d_data_rd_o(mem_d_data_rd_w) //output data
    ,.mem_d_accept_o(mem_d_accept_w)
    ,.mem_d_ack_o(mem_d_ack_w)
    ,.mem_d_error_o(mem_d_error_w)
    ,.mem_d_resp_tag_o(mem_d_resp_tag_w)
);

endmodule