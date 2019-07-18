
%全局变量的声明
global my_uart x  line1 line2;

delete(instrfindall); %删除一些没用的设备，这个很重要

%设置串口的属性
my_uart=serial('COM9','BaudRate',115200); 
set(my_uart,'inputBufferSize',1024000); %设置缓存为1M

 %设置回调函数（相当于中断处理函数）
 set(my_uart,'BytesAvailableFcnMode','byte');%中断触发方式
 set(my_uart,'BytesAvailableFcnCount',10);%中断触发需要接受的字节（实际用的时候，遇到换行就触发中断函数了）
 my_uart.BytesAvailableFcn=@ReceiveCallback;%设置ReceiveCallback为中断函数

%尝试打开串口
try
  fopen(my_uart);  %打开串口
catch err
  fclose(instrfind);  %关闭
  error('Please change Com');  %报错
end

% -------------------------------------动态绘图-----------------------------------------
line1 = animatedline;  %定义一条动态作图线
line2 =animatedline('Color','r');%定义一条颜色为红色的动态线
xlabel('Time');
ylabel('Value');
x=0;

% ---------------------------------------------------------------------

while(1)
    
end
 



