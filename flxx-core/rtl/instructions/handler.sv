module handler (
    input logic [1:0] itype,
    input logic [4:0] opcode,
    input logic [31:0] reg1,
    input logic [31:0] reg2,
    output logic [31:0] outval,
    output logic jmp
);

logic [31:0] alu_out;

flxxalu alu (
    .opcode(opcode[3:0]),
    .reg1(reg1),
    .reg2(reg2),
    .outval(alu_out)
);

always_comb begin
    case (itype)
    00: begin
        if (opcode[4] == 1'b0) begin
            outval = alu_out;
        end
        else begin
            if (opcode[3] == 1'b0) begin
                jmp = (opcode[2] ? (reg1 - reg2)[31] : 0) | (opcode[1] ? ((reg1 - reg2) == 32'b0) : 0) | (opcode[0] ? ~(reg1 - reg2)[31] : 0);
            end 
            else begin
                
            end
        end
    end

    01: begin
        outval = {instruction[23], 8'b0, instruction[22:0]};
    end
    10: begin
        if (opcode[4] == 1'b0) begin
            outval = alu_out;
        end
        else begin
            if (opcode[3] == 1'b0) begin
                jmp = (opcode[2] ? (reg1 - reg2)[31] : 0) | (opcode[1] ? ((reg1 - reg2) == 32'b0) : 0) | (opcode[0] ? !(reg1 - reg2)[31] : 0);
            end 
            else begin
                
            end
        end
    end

    11: begin
        outval = 32'b0;
    end
    endcase
end

endmodule