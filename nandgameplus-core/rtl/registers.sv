module registers (
    input logic clk,
    input logic [15:0] instruction,
    input logic [15:0] result,
    input logic write,
    output logic [15:0] rx_reg,
    output logic [15:0] ry_reg,
    output logic [15:0] t_reg
);

logic [15:0] a_reg, d_reg, sp_reg, zr_reg;

assign zr_reg = 16'b0;

always_comb begin
    rx_reg = (instruction[13] ? (instruction[12] ? zr_reg : sp_reg) : (instruction[12] ? a_reg : d_reg));
    ry_reg = (instruction[11] ? (instruction[10] ? zr_reg : sp_reg) : (instruction[10] ? a_reg : d_reg));
    t_reg = (instruction[1] ? (instruction[0] ? zr_reg : sp_reg) : (instruction[0] ? a_reg : d_reg));
end

always_ff @ (posedge clk) begin
    if (write) begin
        case (instruction[2:0])
        000: d_reg <= result;
        001: a_reg <= result;
        010: sp_reg <= result;
        011:
        endcase 
    end
    
end

    
endmodule