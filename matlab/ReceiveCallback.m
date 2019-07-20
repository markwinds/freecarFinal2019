function ReceiveCallback( obj,event)     %串口接收到数据后的回调函数 
    global my_uart x  line1 line2 line3 x1 y1 y2 y3 count fuck;
    str=fscanf(my_uart,'%s');%从串口读取数据
    %display(str);%显示读取到的数据
    if length(str)==6 && str(6)=='#' %数据为6位，且最后一位是#结束位
        if str(1)=='@' %根据数据头来判断是什么数据
            y=str2num(str(2:5));%得到数据
            x1(count)=x;
            y1(count)=y;
        elseif str(1)=='*'
            y=str2num(str(2:5));%得到数据
            x1(count)=x;
            y2(count)=y;
        elseif str(1)=='$'
            y=str2num(str(2:5));
            x1(count)=x;
            y3(count)=y;
            count=count+1;
            x=x+0.1;
            if count==fuck+1
                count=1;
                addpoints(line1,x1,y1);%添加点到线
                drawnow;%作图
                addpoints(line2,x1,y2);
                drawnow;
                addpoints(line3,x1,y3);
                drawnow;
            end
        end
    end
    if ~isempty(str)
        if str(1)=='#' && str(length(str))=='#'
            title(str(2:length(str)-1));
        end
    end
    %display(str);
end