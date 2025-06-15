module decoder (
    input logic [15:0] instruction,
    input logic [15:0] a_reg,
    input logic [15:0] d_reg,
    input logic [15:0] a_mem_reg,
    output logic [15:0] out,
    output logic jmp,
    output logic [2:0] dst
);

logic [15:0] handle_out;
logic [2:0] handle_dst;
logic handle_jmp;

handler handle (
    .instruction(instruction),
    .a_reg(a_reg),
    .d_reg(d_reg),
    .a_mem_reg(a_mem_reg),
    .out(handle_out),
    .jmp(handle_jmp),
    .dst(handle_dst)
);

always_comb begin
    out = instruction[15] ? handle_out : instruction;
    dst = instruction[15] ? handle_dst : 3'b100;
    jmp = instruction[15] ? handle_jmp : 1'b0;
end

endmodule
