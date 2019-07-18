
%����������
global s h x fuck aim;

delete(instrfindall); %%�ر�û�õģ�������Ҫ

%�½����ڶ���
s=serial('COM11','BaudRate',115200); 
set(s,'inputBufferSize',1024000); %�������뻺������Ϊ1M

 %�����¼��ص�����
 set(s,'BytesAvailableFcnMode','byte');%�����ж���Ӧ��������
 set(s,'BytesAvailableFcnCount',10);%�����ն˴�����ʽ ���趨��������ﵽ10���ֽڣ��ʹ����жϣ����ûص����������д�����
 s.BytesAvailableFcn=@ReceiveCallback;%ReceiveCallback���жϵĴ������������������Զ���ġ�ϵͳ�Ļص�����Ϊinstrcallback;

%���Դ�������
try
  fopen(s);  %�򿪴��ڶ���
catch err
  fclose(instrfind);  %�رձ�ռ�õĴ���
  error('��ȷ��ѡ������ȷ�Ĵ���');  %���������ʾ
end

% -------------------------------------����̬ͼ-----------------------------------------
h = animatedline;  %�����հ׶�̬����
%axis([0 40 5 30])  %��������ϵ��Χ
%x = linspace(0,40,800);
aim =animatedline('Color','r');
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
 
 %data=fread(s)%��ȡ�������ֽ� fwrite������д�� ��Ӧ�ı���fscanf ��fpintf



