`include "try1.v"
module tb_try1();

reg [4:0] var1;
reg [0:3] var2;


localparam param1 = 3;
localparam param2 = 4;

try1 #(.param1(param1), .param2(param2) )DUT(.var1(var1), .var2(var2) );


initial begin
$dumpfile("tb_try1.vcd");
$dumpvars(0,tb_try1);
#10
$finish;




end



endmodule
