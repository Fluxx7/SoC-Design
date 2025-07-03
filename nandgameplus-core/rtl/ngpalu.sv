module ngpalu (
    input logic [3:0] opcode,
    input logic [15:0] x,
    input logic [15:0] y,
    output logic [15:0] outval
);

always_comb begin
    case (opcode)
    0000: begin
        outval <= x & y;
    end
    0001: begin
        outval <= x | y;
    end
    0010: begin
        outval <= x ^ y;
    end
    0011: begin
        outval <= ~x;
    end
    0100: begin
        outval <= x >> y;
    end
    0101: begin
        outval <= x << y;
    end

    0110: begin
        outval <= x >>> y;
    end
    1000: begin
        outval <= x + y;
    end
    1001: begin
        outval <= x - y;
    end
    1010: begin
        outval <= x + (y + 1);
    end
    1011: begin
        outval <= x - (y + 1);
    end
    default: begin

    end
    endcase
end

endmodule