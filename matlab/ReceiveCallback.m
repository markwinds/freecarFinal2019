function ReceiveCallback( obj,event)     %�����ж���Ӧ���� 
    global str_data s x h;
    temp_str=fscanf(s,'%s');%��ȡ�ַ���
    str_data=[str_data,temp_str];%ƴ���ַ���
    for i= 1:length(str_data)-6
        if str_data(i)=='@'
            if str_data(i+5)=='#'
                y=str2num(str_data(i+1:i+4));
                x=x+0.1;
                addpoints(h,x,y);
                drawnow;
            end
        end
    end
    str_data=str_data(length(str_data)-5:length(str_data));
    disp(str_data);
end