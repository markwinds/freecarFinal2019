function ReceiveCallback( obj,event)     %串口接收到数据后的回调函数 
    global my_uart x  line1 line2;
    str=fscanf(my_uart,'%s');%从串口读取数据
    display(str);%显示读取到的数据
    if length(str)==6 && str(6)=='#' %数据为6位，且最后一位是#结束位
        if str(1)=='@' %根据数据头来判断是什么数据
            y=str2num(str(2:5));%得到数据
            addpoints(line1,x,y);%添加点到线
            drawnow;%作图
            x=x+0.1;
        elseif str(1)=='$'
            y=str2num(str(2:5));
            addpoints(line2,x,y);
            drawnow;
        end
    end
end