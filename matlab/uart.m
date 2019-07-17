
%±äÁ¿µÄÉùÃ÷
<<<<<<< HEAD
global s h x;
=======
global s;
>>>>>>> å†™è·¯éšœ

delete(instrfindall); %%¹Ø±ÕÃ»ÓÃµÄ£¬Õâ¾äºÜÖØÒª

%ĞÂ½¨´®¿Ú¶ÔÏó
s=serial('COM9','BaudRate',115200); 
set(s,'inputBufferSize',1024000); %ÉèÖÃÊäÈë»º³åÇøÓòÎª1M

 %´®¿ÚÊÂ¼ş»Øµ÷ÉèÖÃ
<<<<<<< HEAD
 set(s,'BytesAvailableFcnMode','byte');%ÉèÖÃÖĞ¶ÏÏìÓ¦º¯Êı¶ÔÏó
 set(s,'BytesAvailableFcnCount',10);%ÉèÖÃÖÕ¶Ë´¥·¢·½Ê½ ÈçÉè¶¨»º´æÇøÓò´ïµ½10¸ö×Ö½Ú£¬¾Í´¥·¢ÖĞ¶Ï£¬µ÷ÓÃ»Øµ÷º¯Êı£¬½øĞĞ´¦Àí¡£
 s.BytesAvailableFcn=@ReceiveCallback;%ReceiveCallbackÊÇÖĞ¶ÏµÄ´¥·¢º¯Êı£¬ÕâÀïÎÒÊÇ×Ô¶¨ÒåµÄ¡£ÏµÍ³µÄ»Øµ÷º¯ÊıÎªinstrcallback;
=======
 set(s,'BytesAvailabelFcnMode','bytes');%ÉèÖÃÖĞ¶ÏÏìÓ¦º¯Êı¶ÔÏó
 set(s,'BytesAvailabelFcnCount',10);%ÉèÖÃÖÕ¶Ë´¥·¢·½Ê½ ÈçÉè¶¨»º´æÇøÓò´ïµ½10¸ö×Ö½Ú£¬¾Í´¥·¢ÖĞ¶Ï£¬µ÷ÓÃ»Øµ÷º¯Êı£¬½øĞĞ´¦Àí¡£
 s.BytesAvailabelFcn=@ReceiveCallback;%ReceiveCallbackÊÇÖĞ¶ÏµÄ´¥·¢º¯Êı£¬ÕâÀïÎÒÊÇ×Ô¶¨ÒåµÄ¡£ÏµÍ³µÄ»Øµ÷º¯ÊıÎªinstrcallback;
>>>>>>> å†™è·¯éšœ

%³¢ÊÔ´ú¿ª´®¿Ú
try
  fopen(s);  %´ò¿ª´®¿Ú¶ÔÏó
catch err
  fclose(instrfind);  %¹Ø±Õ±»Õ¼ÓÃµÄ´®¿Ú
  error('ÇëÈ·ÈÏÑ¡ÔñÁËÕıÈ·µÄ´®¿Ú');  %Êä³ö´íÎóÌáÊ¾
end

<<<<<<< HEAD
% -------------------------------------»­¶¯Ì¬Í¼-----------------------------------------
h = animatedline;  %´´½¨¿Õ°×¶¯Ì¬ÇúÏß
%axis([0 40 5 30])  %ÉèÖÃ×ø±êÏµ·¶Î§
%x = linspace(0,40,800);
xlabel('Ê±¼ä');
ylabel('ËÙ¶È');
x=0;
% for k = 1:length(x)
%     y = (fscanf(arduino,'%d')/100.0);
%     addpoints(h,x(k),y);
%     drawnow
% end

% ---------------------------------------------------------------------

while(1)
    
end
=======


>>>>>>> å†™è·¯éšœ
 
 %data=fread(s)%¶ÁÈ¡¶ş½øÖÆ×Ö½Ú fwrite¶ş½øÖÆĞ´Èë ÏàÓ¦ÎÄ±¾ÓÃfscanf ºÍfpintf



