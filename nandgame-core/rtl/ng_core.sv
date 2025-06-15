module ng_core (
    input logic clk,
    input logic rst,
    input logic [15:0] instruction,
    output logic [15:0] addr
);

logic [15:0] a_reg, d_reg, a_mem_reg, result;
logic [2:0] write;
logic jmp;

memory mem (
    .clk(clk),
    .x(result),
    .write(write),
    .a_mem_reg(a_mem_reg),
    .a_reg(a_reg),
    .d_reg(d_reg)
);

decoder control (
    .instruction(instruction),
    .a_reg(a_reg),
    .d_reg(d_reg),
    .a_mem_reg(a_mem_reg),
    .out(result),
    .jmp(jmp),
    .dst(write)
);

always_ff @ (posedge clk or negedge rst) begin
    if (!rst) begin
        addr <= 0;
    end
    else begin
        if (jmp) begin
            addr <= a_reg;
        end
        else begin
            addr <= addr + 16'b1;
        end
    end
end

endmodule
