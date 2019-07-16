
%变量的声明
global s;

delete(instrfindall); %%关闭没用的，这句很重要

%新建串口对象
s=serial('COM9','BaudRate',115200); 
set(s,'inputBufferSize',1024000); %设置输入缓冲区域为1M

 %串口事件回调设置
 set(s,'BytesAvailabelFcnMode','bytes');%设置中断响应函数对象
 set(s,'BytesAvailabelFcnCount',10);%设置终端触发方式 如设定缓存区域达到10个字节，就触发中断，调用回调函数，进行处理。
 s.BytesAvailabelFcn=@ReceiveCallback;%ReceiveCallback是中断的触发函数，这里我是自定义的。系统的回调函数为instrcallback;

%尝试代开串口
try
  fopen(s);  %打开串口对象
catch err
  fclose(instrfind);  %关闭被占用的串口
  error('请确认选择了正确的串口');  %输出错误提示
end



 
 %data=fread(s)%读取二进制字节 fwrite二进制写入 相应文本用fscanf 和fpintf



