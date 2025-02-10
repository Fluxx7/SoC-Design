module flxx_core (
    input logic clk,
    input logic rst_n
);
logic [15:0][31:0] regarray; // rz, r0, r1, r2, r3, r4, r5, r6, i0, i1, i2, i3, ir, ra, rs, rc
logic [15:0][31:0] mem_regarray;
logic [31:0] instruction;
logic [1:0][31:0] ro_regarray; // stack pointer, instruction pointer
logic [31:0] write_val, write_addr;
logic [31:0] result;
logic [5:0] target;
logic _rst, write;

assign _rst = !rst_n;

assign regarray[0] = 32'b0;

memory mem (
    .clk(clk),
    ._rst(_rst),
    .regarray({ro_regarray[1], regarray[15:1], ro_regarray[0]}),
    .write(write),
    .write_addr(write_addr),
    .write_val(write_val),
    .mem_regarray({instruction, mem_regarray})
);

decoder control (
    .instruction(instruction),
    .regarray(regarray),
    .mem_regarray(mem_regarray),
    .outval(result),
    .outreg(target)
);

always_ff @ (posedge clk or posedge rst_n) begin
    if (rst_n) begin
        ro_regarray[1] <= 32'b0;
        ro_regarray[0] <= 32'd1024;
    end
    else begin 
        ro_regarray[1] <= target[5] ? 
            target[4] ? mem_regarray[target[3:0]] : regarray[target[3:0]] : 
            ro_regarray[1] + 32'b1;
        if (!target[5]) begin
            if (target[4]) begin 
                write_addr <= regarray[target[3:0]];
                write_val <= result;
                write <= 1'b1;
            end
            else if (target[3:0] != 4'b0000) begin 
                regarray[target[3:0]] <= result;
                write <= 1'b0;
            end
        end
    end
end

endmodule