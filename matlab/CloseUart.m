function CloseUart%�رմ���
    global s;
    fclose(s);
    delete(s);
    clear s;
    clear all;
end