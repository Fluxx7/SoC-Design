module decoder (
    input logic [31:0] instruction,
    input logic [15:0][31:0] regarray,
    input logic [15:0][31:0] mem_regarray
    output logic [31:0] outval,
    output logic [5:0] outreg
);

logic [31:0] reg1, reg2;
logic jmp;

handler handle (
    .itype(instruction[31:30]),
    .opcode(instruction[4:0]),
    .reg1(reg1),
    .reg2(reg2),
    .outval(outval),
    .jmp(jmp)
);

always_comb begin
    case (instruction[31:30])
    00: begin
        reg1 = instruction[28] ? mem_regarray[instruction[27:24]] : regarray[instruction[27:24]];
        reg2 = instruction[23] ? mem_regarray[instruction[22:19]] : regarray[instruction[22:19]];
        outreg = {jmp, instruction[9:5]};
    end

    01: begin 
        reg1 = 32'b0;
        reg2 = 32'b0;
        outreg = {1'b0, instruction[27:24]};
    end

    10: begin
        case (instruction[29])
        0: begin
            reg1 = instruction[28] ? mem_regarray[instruction[27:24]] : regarray[instruction[27:24]];
            reg2 = {instruction[23], 18'b0, instruction[22:10]};
        end
        1: begin
            reg2 = instruction[28] ? mem_regarray[instruction[27:24]] : regarray[instruction[27:24]];
            reg1 = {instruction[23], 18'b0, instruction[22:10]};
        end
        endcase
        outreg = {jmp, instruction[9:5]};
    end

    11: begin
        reg1 = 32'b0;
        reg2 = 32'b0;
        outreg = 6'b0;
    end
    endcase
end

endmodule