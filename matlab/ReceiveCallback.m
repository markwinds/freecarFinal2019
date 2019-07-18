function ReceiveCallback( obj,event)     %�����ж���Ӧ���� 
    global str_data s x h aim;
%     temp_str=fscanf(s,'%s');%��ȡ�ַ���
%     str_data=[str_data,temp_str];%ƴ���ַ���
%     for i= 1:length(str_data)-6
%         if str_data(i)=='@'
%             if str_data(i+5)=='#'
%                 y=str2num(str_data(i+1:i+4));
%                 x=x+0.1;
%                 addpoints(h,x,y);
%                 drawnow;
%             end
%         end
%     end
%     str_data=str_data(length(str_data)-5:length(str_data));
%     disp(str_data);
    str=fscanf(s,'%s');
    display(str);
    if length(str)==6 && str(6)=='#'
        if str(1)=='@'
            y=str2num(str(2:5));
            addpoints(h,x,y);
            drawnow;
            x=x+0.1;
        elseif str(1)=='$'
            y=str2num(str(2:5));
            addpoints(aim,x,y);
            drawnow;
        end
    end
end