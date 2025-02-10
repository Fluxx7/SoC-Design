module ngpalu (
    input logic [2:0] opcode,
    input logic zy,
    input logic [15:0] rx_reg,
    input logic [15:0] ry_reg,
    output logic [15:0] outval
);


assign y = zy ? 16'b0 : ry_reg;

always_comb begin
    case (opcode[2])
    0: begin
        case (opcode[1:0])
            00: begin
                outval = rx_reg & y; 
            end
            01: begin
                outval = rx_reg | y;
            end
            10: begin
                outval = rx_reg ^ y;
            end
            11: begin
                outval = ~rx_reg;
            end
        endcase
    end
    1: begin
        case (opcode[1:0])
            00: begin
                outval = rx_reg + y; 
            end
            01: begin
                outval = rx_reg - y;
            end
            10: begin
                outval = rx_reg + 1;
            end
            11: begin
                outval = rx_reg - 1;
            end
        endcase
    end
    endcase
end

endmodule