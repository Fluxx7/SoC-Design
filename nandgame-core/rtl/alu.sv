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

always_comb begin
    case (opcode[2])
    0: begin
        case (opcode[1:0])
            00: begin
                outval = x & y; 
            end
            01: begin
                outval = x | y;
            end
            10: begin
                outval = x ^ y;
            end
            11: begin
                outval = ~x;
            end
        endcase
    end
    1: begin
        case (opcode[1:0])
            00: begin
                outval = x + y; 
            end
            01: begin
                outval = x - y;
            end
            10: begin
                outval = x + 1;
            end
            11: begin
                outval = x - 1;
            end
        endcase
    end
    endcase
end

endmodule