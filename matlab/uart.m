
%全局变量的声明
global my_uart x  line1 line2 line3 x1 y1 y2 y3 count o c fuck;

delete(instrfindall); %删除一些没用的设备，这个很重要

%设置串口的属性
my_uart=serial('COM11','BaudRate',115200); 
set(my_uart,'inputBufferSize',1024000); %设置缓存为1M

 %设置回调函数（相当于中断处理函数）
 set(my_uart,'BytesAvailableFcnMode','byte');%中断触发方式
 set(my_uart,'BytesAvailableFcnCount',20);%中断触发需要接受的字节（实际用的时候，遇到换行就触发中断函数了）
 set(my_uart,'Timeout',100);
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
line2=animatedline('Color','b');
line3 =animatedline('Color','r');%定义一条颜色为红色的动态线
xlabel('Time');
ylabel('Value');
xticks(0:10:5000);
%yticks(-400:100:400);
grid on;
grid minor;

%--------------------参数初始化------------------
x=0;
count=1;
fuck=5;
x1=zeros(1,fuck);
y1=zeros(1,fuck);
y2=zeros(1,fuck);
y3=zeros(1,fuck);
o='o';
c='c';
r='reset';
n='';

% ---------------------------------------------------------------------

while(1)
    cmd=input('command:');
%     if length(cmd)~=0
%         if cmd==r
%           clearpoints(line1);
%           clearpoints(line2);
%         else
%             fprintf(my_uart,'%s#',cmd);
%         end
%     else
%         fprintf(my_uart,'%s#',cmd);
%     end
    if isempty(cmd)
        count=1;
        clearpoints(line1);
        clearpoints(line2);
        clearpoints(line3);
    end
end
 





