module handler (
    input logic [15:0] instruction,
    input logic [15:0] rx_reg,
    input logic [15:0] ry_reg,
    input logic [15:0] rx_mem_reg,
    output logic [15:0] out,
    output logic jmp,
    output logic [2:0] dst
);

logic lt, eq, gt;

ngpalu alu (
    .opcode(instruction[6:4]),
    .zy(instruction[3]),
    .rx_reg(instruction[14] ? rx_mem_reg : rx_reg),
    .ry_reg(ry_reg),
    .outval(out)
);

always_comb begin
    lt = out[15];
    eq = (out == 16'b0);
    gt = ~lt & ~eq;
end

assign dst = instruction[9:7];
assign jmp = (instruction[2] ? lt : 0) | (instruction[1] ? eq : 0) | (instruction[0] ? gt : 0);

endmodule
