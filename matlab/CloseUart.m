function CloseUart%关闭串口
    global my_uart;
    fclose(my_uart);
    delete(my_uart);
    clear my_uart;
    clear all;
end