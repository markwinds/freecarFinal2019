
%����������
global s;

delete(instrfindall); %%�ر�û�õģ�������Ҫ

%�½����ڶ���
s=serial('COM9','BaudRate',115200); 
set(s,'inputBufferSize',1024000); %�������뻺������Ϊ1M

 %�����¼��ص�����
 set(s,'BytesAvailabelFcnMode','bytes');%�����ж���Ӧ��������
 set(s,'BytesAvailabelFcnCount',10);%�����ն˴�����ʽ ���趨��������ﵽ10���ֽڣ��ʹ����жϣ����ûص����������д���
 s.BytesAvailabelFcn=@ReceiveCallback;%ReceiveCallback���жϵĴ������������������Զ���ġ�ϵͳ�Ļص�����Ϊinstrcallback;

%���Դ�������
try
  fopen(s);  %�򿪴��ڶ���
catch err
  fclose(instrfind);  %�رձ�ռ�õĴ���
  error('��ȷ��ѡ������ȷ�Ĵ���');  %���������ʾ
end



 
 %data=fread(s)%��ȡ�������ֽ� fwrite������д�� ��Ӧ�ı���fscanf ��fpintf



