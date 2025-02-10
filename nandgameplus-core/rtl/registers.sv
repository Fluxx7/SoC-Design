module registers (
    input logic clk,
    input logic [15:0] instruction,
    input logic [15:0] x,
    input logic [2:0] write,
    output logic [15:0] rx_reg,
    output logic [15:0] ry_reg,
    output logic [15:0] t_reg
);

logic [15:0] a_reg, d_reg, sp_reg, ra_reg;

always_comb begin
    if (!instruction[14]) begin
        rx_reg = (instruction[13] ? (instruction[12] ? ra_reg : sp_reg) : (instruction[12] ? d_reg : a_reg));
    end
    ry_reg = (instruction[11] ? (instruction[10] ? ra_reg : sp_reg) : (instruction[10] ? d_reg : a_reg));
    t_reg = (instruction[9] ? (instruction[8] ? ra_reg : sp_reg) : (instruction[8] ? d_reg : a_reg));
end

always_ff @ (posedge clk) begin
    if (!instruction[15]) begin
        case (instruction[14:12])
        000: a_reg <= instruction;
        001: d_reg <= instruction;
        010: sp_reg <= instruction;
        011: ra_reg <= instruction;
        endcase 
    end
    else begin
        case (instruction[10:8])
        000: a_reg <= x;
        001: d_reg <= x;
        010: sp_reg <= x;
        011: ra_reg <= x;
        endcase 
    end
    
end

    
endmodule