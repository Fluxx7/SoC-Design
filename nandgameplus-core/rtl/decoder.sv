module decoder (
    input logic [15:0] instruction,
    input logic [15:0] rx_reg,
    input logic [15:0] ry_reg,
    input logic [15:0] rx_mem_reg,
    output logic [15:0] out,
    output logic jmp,
    output logic [2:0] dst
);

logic [15:0] handle_out;
logic [2:0] handle_dst;
logic handle_jmp;

handler handle (
    .instruction(instruction),
    .rx_reg(rx_reg),
    .ry_reg(ry_reg),
    .rx_mem_reg(rx_mem_reg),
    .out(handle_out),
    .jmp(handle_jmp),
    .dst(dst)
);

always_comb begin
    out = instruction[15] ? handle_out : instruction;
    dst = instruction[15] ? handle_dst : 3'b000;
    jmp = instruction[15] ? handle_jmp : 1'b0;
end

endmodule