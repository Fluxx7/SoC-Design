module flxxalu (
    input logic [3:0] opcode,
    input logic [31:0] reg1,
    input logic [31:0] reg2,
    output logic [31:0] outval
);

always_comb begin
    case (opcode[3:2])
    00: begin
        case (opcode[1:0])
            00: begin
                outval = reg1 & reg2; 
            end
            01: begin
                outval = reg1 | reg2;
            end
            10: begin
                outval = reg1 ^ reg2;
            end
            11: begin
                outval = ~reg1;
            end
        endcase
    end
    01: begin
        case (opcode[1:0])
            00: begin
                outval = reg1 + reg2; 
            end
            01: begin
                outval = reg1 - reg2;
            end
            10: begin
                outval = reg1 << reg2;
            end
            11: begin
                outval = reg1 >> reg2;
            end
        endcase
    end
    10: begin
        case (opcode[1:0])
            10: begin
                outval = reg1 <<< reg2;
            end
            11: begin
                outval = reg1 >>> reg2;
            end
            default: outval = 32'b0;
        endcase
    end
    default: outval = 32'b0;
    endcase
end

endmodule
