module ngp_core (
    input logic clk,
    input logic _rst,
    input logic [15:0] instruction,
    output logic [15:0] addr
);

logic [15:0] rx_reg, ry_reg, rx_mem_reg;
logic [15:0] t_reg, t_mem_reg, result;
logic jmp, write_reg, write_mem;

registers reg (
    .clk(clk),
    .instruction(instruction),
    .write(write_reg),
    .result(result),
    .rx_reg(rx_reg),
    .ry_reg(ry_reg),
    .t_reg(t_reg)
);

memory mem (
    .clk(clk),
    .write_val(result),
    .x_reg(rx_reg),
    .t_reg(t_reg),
    .write(write_mem),
    .mem_reg(rx_mem_reg),
    .t_val(t_mem_reg)
);

decoder decode (
    .instruction(instruction),
    .x_reg(rx_reg),
    .y_reg(ry_reg),
    .mem_reg(rx_mem_reg),
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