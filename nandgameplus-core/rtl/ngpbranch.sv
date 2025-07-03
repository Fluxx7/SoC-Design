module ngpbranch (
    input logic [2:0] brcode,
    input logic [15:0] rx_reg,
    input logic [15:0] ry_reg,
    output logic jmp
);

logic [15:0] x, y;
logic lt, eq, gt;

assign x = sw ? ry_reg : rx_reg;
assign y = sw ? rx_reg : ry_reg;

always_comb begin
    gt = x > y;
    eq = x == y;
    lt = x < y;
    jmp = brcode[2] & gt | brcode[1] & eq | brcode[0] & lt;
end

endmodule