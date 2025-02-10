module memory #(parameter MEM_SIZE=4096) (
    input logic clk,
    input logic [15:0] x,
    input logic [15:0] rx_reg,
    input logic [15:0] t_reg,
    input logic write,
    output logic [15:0] rx_mem_reg
);

logic [(MEM_SIZE-1):0][15:0] mem_array;

always_ff @ (posedge clk) begin
    rx_mem_reg <= mem_array[rx_reg];
    if (write) begin
        mem_array[t_reg] <= x;
    end
end


endmodule