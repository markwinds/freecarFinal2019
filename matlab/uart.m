
%����������
<<<<<<< HEAD
global s h x;
=======
global s;
>>>>>>> 写路障

delete(instrfindall); %%�ر�û�õģ�������Ҫ

%�½����ڶ���
s=serial('COM9','BaudRate',115200); 
set(s,'inputBufferSize',1024000); %�������뻺������Ϊ1M

 %�����¼��ص�����
<<<<<<< HEAD
 set(s,'BytesAvailableFcnMode','byte');%�����ж���Ӧ��������
 set(s,'BytesAvailableFcnCount',10);%�����ն˴�����ʽ ���趨��������ﵽ10���ֽڣ��ʹ����жϣ����ûص����������д���
 s.BytesAvailableFcn=@ReceiveCallback;%ReceiveCallback���жϵĴ������������������Զ���ġ�ϵͳ�Ļص�����Ϊinstrcallback;
=======
 set(s,'BytesAvailabelFcnMode','bytes');%�����ж���Ӧ��������
 set(s,'BytesAvailabelFcnCount',10);%�����ն˴�����ʽ ���趨��������ﵽ10���ֽڣ��ʹ����жϣ����ûص����������д���
 s.BytesAvailabelFcn=@ReceiveCallback;%ReceiveCallback���жϵĴ������������������Զ���ġ�ϵͳ�Ļص�����Ϊinstrcallback;
>>>>>>> 写路障

%���Դ�������
try
  fopen(s);  %�򿪴��ڶ���
catch err
  fclose(instrfind);  %�رձ�ռ�õĴ���
  error('��ȷ��ѡ������ȷ�Ĵ���');  %���������ʾ
end

<<<<<<< HEAD
% -------------------------------------����̬ͼ-----------------------------------------
h = animatedline;  %�����հ׶�̬����
%axis([0 40 5 30])  %��������ϵ��Χ
%x = linspace(0,40,800);
xlabel('ʱ��');
ylabel('�ٶ�');
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


>>>>>>> 写路障
 
 %data=fread(s)%��ȡ�������ֽ� fwrite������д�� ��Ӧ�ı���fscanf ��fpintf



