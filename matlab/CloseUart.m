function CloseUart%¹Ø±Õ´®¿Ú
    global s;
    fclose(s);
    delete(s);
    clear s;
    clear all;
end