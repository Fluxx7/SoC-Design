module ngalu (
    input logic [2:0] opcode,
    input logic zx,
    input logic sw,
    input logic [15:0] reg1,
    input logic [15:0] reg2,
    output logic [15:0] outval
);

logic [15:0] x, y;

assign x = zx ? 16'b0 : (sw ? reg2 : reg1);
assign y = sw ? reg1 : reg2;

/* verilator lint_off WIDTHEXPAND */
always_comb begin
    case (opcode[2])
    1'b0: begin
        case (opcode[1:0])
            2'b00: begin
                outval = x & y; 
            end
            2'b01: begin
                outval = x | y;
            end
            2'b10: begin
                outval = x ^ y;
            end
            2'b11: begin
                outval = ~x;
            end
        endcase
    end
    1'b1: begin
        case (opcode[1:0])
            2'b00: begin
                outval = x + y; 
            end
            2'b01: begin
                outval = x + 16'b1; 
            end
            2'b10: begin
                outval = x + (~y+1);
            end
            2'b11: begin
                outval = x + 16'hffff; 
            end
        endcase
    end
    endcase
end


/* verilator lint_on WIDTHEXPAND */
endmodule
