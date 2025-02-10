module handler (
    input logic [15:0] instruction,
    input logic [15:0] a_reg,
    input logic [15:0] d_reg,
    input logic [15:0] a_mem_reg,
    output logic [15:0] out,
    output logic jmp,
    output logic [2:0] dst
);

logic lt, eq, gt;

ngalu alu (
    .opcode(instruction[10:8]),
    .zx(instruction[7]),
    .sw(instruction[6])
    .reg1(d_reg),
    .reg2(instruction[12] ? a_mem_reg : a_reg),
    .outval(out)
);

always_comb begin
    lt = out[15];
    eq = (out == 16'b0);
    gt = ~lt & ~eq;
end

assign dst = instruction[5:3];
assign jmp = (instruction[2] ? lt : 0) | (instruction[1] ? eq : 0) | (instruction[0] ? gt : 0);

endmodule
