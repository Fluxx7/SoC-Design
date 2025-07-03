module memory #(parameter MEM_SIZE=65536) (
    input logic clk,
    input logic [15:0] write_val,
    input logic [15:0] x_reg,
    input logic [15:0] t_reg,
    input logic write,
    output logic [15:0] mem_reg,
    output logic [15:0] t_val
);

logic [(MEM_SIZE-1):0][15:0] mem_array;


always_comb begin
    mem_reg = mem_array[x_reg];
    t_val = mem_array[t_reg];
end




always_ff @ (posedge clk) begin
    if (write) begin
        mem_array[t_reg] <= write_val;
    end
end


endmodule