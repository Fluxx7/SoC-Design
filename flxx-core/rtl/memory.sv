module memory #(parameter MEM_SIZE=4096) (
    input logic clk,
    input logic _rst,
    input logic [16:0][31:0] regarray,
    input logic write,
    input logic [31:0] write_addr,
    input logic [31:0] write_val,
    output logic [16:0][31:0] mem_regarray
);

logic [(MEM_SIZE-1):0][31:0] mem_array;

always_comb begin
    // top of stack
    mem_regarray[0] = mem_array[regarray[0][11:0]]; 
    // generic memory access
    mem_regarray[1] = mem_array[regarray[1][11:0]]; 
    mem_regarray[2] = mem_array[regarray[2][11:0]]; 
    mem_regarray[3] = mem_array[regarray[3][11:0]]; 
    mem_regarray[4] = mem_array[regarray[4][11:0]]; 
    mem_regarray[5] = mem_array[regarray[5][11:0]]; 
    mem_regarray[6] = mem_array[regarray[6][11:0]]; 
    mem_regarray[7] = mem_array[regarray[7][11:0]]; 
    mem_regarray[8] = mem_array[regarray[8][11:0]]; 
    mem_regarray[9] = mem_array[regarray[9][11:0]]; 
    mem_regarray[10] = mem_array[regarray[10][11:0]]; 
    mem_regarray[11] = mem_array[regarray[11][11:0]]; 
    mem_regarray[12] = mem_array[regarray[12][11:0]]; 
    mem_regarray[13] = mem_array[regarray[13][11:0]]; 
    mem_regarray[14] = mem_array[regarray[14][11:0]]; 
    mem_regarray[15] = mem_array[regarray[15][11:0]]; 
    // current instruction
    mem_regarray[16] = mem_array[regarray[16][11:0]]; 
end

always_ff @ (posedge clk or negedge _rst) begin
    if (!_rst) begin
        
    end else begin
        if (write) begin
            mem_array[write_addr] <= write_val;
        end
    end
end

    
endmodule