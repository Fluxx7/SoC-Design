module ngp_core (
    input logic clk,
    input logic [15:0] instruction,
    output logic [15:0] addr
);

logic [15:0] rx_reg, ry_reg, rx_mem_reg, t_reg, result;
logic [2:0] write;
logic jmp;

registers reg (
    .clk(clk),
    .instruction(instruction),
    .write(write),
    .rx_reg(rx_reg),
    .ry_reg(ry_reg),
    .t_reg(t_reg)
);

memory mem (
    .clk(clk),
    .x(result),
    .rx_reg(rx_reg),
    .t_reg(t_reg),
    .write(write[2]),
    .rx_mem_reg(rx_mem_reg)
);

decoder control (
    .instruction(instruction),
    .a_reg(a_reg),
    .d_reg(d_reg),
    .r_mem_reg(r_mem_reg),
    .out(result),
    .jmp(jmp),
    .dst(write)
);

always_ff @ (posedge clk) begin
    if (jmp) begin
        addr <= t_reg;
    end
    else begin
        addr <= addr + 16'b1;
    end
end

endmodule