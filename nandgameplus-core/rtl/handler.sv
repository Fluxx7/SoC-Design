module handler (
    input logic [15:0] instruction,
    input logic [15:0] rx_reg,
    input logic [15:0] ry_reg,
    input logic [15:0] rx_mem_reg,
    output logic [15:0] out,
    output logic jmp,
    output logic [2:0] dst
);

logic [15:0] x, y, alu_out;

ngpalu alu (
    .opcode(instruction[8:5]),
    .rx_reg(x),
    .ry_reg(y),
    .outval(alu_out)
);

ngpbranch branch (
    .brcode(instruction[7:5]),
    .rx_reg(x),
    .ry_reg(y),
    .outval(alu_out)
);

always_comb begin
    x = instruction[9] ? ry_reg : rx_reg;
    y = instruction[9] ? rx_reg : ry_reg;
    if (instruction[8:5] == 0111) begin
        
    end
    else begin
        out = alu_out;
    end
end 

endmodule
