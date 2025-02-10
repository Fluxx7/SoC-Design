module ng_core_tb;
logic [15:0] addr, instruction;
logic clk;

ng_core dut(.clk(clk),
        .instruction(instruction),
        .addr(addr));

always
begin
   clk = 1'b0;
   #5 clk = 1'b1;
   #5;
end

integer file;

initial
begin
   file = $fopen("code.txt", "r")
   $finish;
end

endmodule