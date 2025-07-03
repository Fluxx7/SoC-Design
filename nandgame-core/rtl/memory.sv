module memory #(parameter MEM_SIZE=65536) (
    input logic clk,
    input logic [15:0] x,
    input logic [2:0] write,
    output logic [15:0] a_mem_reg,
    output logic [15:0] a_reg,
    output logic [15:0] d_reg
);

logic [(MEM_SIZE-1):0][15:0] mem_array;

always_ff @ (posedge clk) begin
    a_mem_reg <= mem_array[a_reg];
    if (write[0]) begin
        mem_array[a_reg] <= x;
    end
    if (write[1]) begin
        d_reg <= x;
    end
    if (write[2]) begin
        a_reg <= x;
    end
end

    
endmodule
