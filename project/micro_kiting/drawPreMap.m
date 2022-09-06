%% 适用于Matlab的RGB颜色
% [0.00,0.45,0.74] 蓝
% [0.85,0.33,0.10] 橙红
% [0.93,0.69,0.13] 橙黄
% [0.72,0.27,1]    淡紫
% [0.47,0.67,0.19] 淡绿
%%
%最优解示意图
clc;
clear
close all;

filename = 'D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_pre_map_6enemy.txt';
fileID = fopen(filename);
C = textscan(fileID,'%f %f %f %f %f %f %f %f','Delimiter',',');
fclose(fileID);
column1 = [C{1},C{2}];
column2 = [C{3},C{4}];
column3 = [C{5},C{6}];
column4 = [C{7},C{8}];
arg_1=[mean(C{1}),mean(C{2})];
arg_2=[mean(C{3}),mean(C{4})];
arg_3=[mean(C{5}),mean(C{6})];
arg_4=[mean(C{7}),mean(C{8})];
figure(1)
for i=1:100
    scatter(column1(i,1),column1(i,2),'MarkerEdgeColor',[0.85,0.33,0.10],'MarkerFaceColor',[0.85,0.33,0.10])
    hold on;  
    scatter(column2(i,1),column1(i,2),'MarkerEdgeColor',[0.93,0.69,0.13],'MarkerFaceColor',[0.93,0.69,0.13])
    hold on;  
    scatter(column3(i,1),column1(i,2),'MarkerEdgeColor',[0.47,0.67,0.19],'MarkerFaceColor',[0.47,0.67,0.19])
    hold on;  
    scatter(column4(i,1),column1(i,2),'MarkerEdgeColor',[0.00,0.45,0.74],'MarkerFaceColor',[0.00,0.45,0.74])
end
axis([45 60 60 70])

figure(2)
scatter(arg_1(1,1),arg_1(1,2),'MarkerEdgeColor',[0.85,0.33,0.10],'MarkerFaceColor',[0.85,0.33,0.10])
hold on;  
scatter(arg_2(1,1),column1(1,2),'MarkerEdgeColor',[0.93,0.69,0.13],'MarkerFaceColor',[0.93,0.69,0.13])
hold on;  
scatter(arg_3(1,1),column1(1,2),'MarkerEdgeColor',[0.47,0.67,0.19],'MarkerFaceColor',[0.47,0.67,0.19])
hold on;  
scatter(arg_4(1,1),column1(1,2),'MarkerEdgeColor',[0.00,0.45,0.74],'MarkerFaceColor',[0.00,0.45,0.74]) 
axis([45 60 60 70])
%%
%最优解示意图
clc;
clear

figure(3)
filename = 'D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_pre_map_pre.txt';
kitingpremap = importdata(filename);
for i=1:18
    eval(['column',num2str(i),'=','[kitingpremap(:,',num2str(2*i-1),'),kitingpremap(:,',num2str(2*i),')];']);
end
for i=1:18
    eval(['arg_',num2str(i),'=','[mean(column',num2str(i),'(:,',num2str(1),')),mean(column',num2str(i),'(:,',num2str(2),'))];']);
end
for k=1:100
    for i=1:18
        color = [i/18 i/18 1];
        eval(['scatter(column',num2str(i),'(',num2str(k),',1),column',num2str(i),'(',num2str(k),',2),''MarkerEdgeColor'',color,''MarkerFaceColor'',color);']);
        hold on;
%         plot([column1(k,1),column2(k,1),column3(k,1),column4(k,1),column5(k,1),column6(k,1),column7(k,1),column8(k,1),column9(k,1),column10(k,1),column11(k,1),column12(k,1),column13(k,1),column14(k,1),column15(k,1),column16(k,1),column17(k,1),column18(k,1)],[column1(k,2),column2(k,2),column3(k,2),column4(k,2),column5(k,2),column6(k,2),column7(k,2),column8(k,2),column9(k,2),column10(k,2),column11(k,2),column12(k,2),column13(k,2),column14(k,2),column15(k,2),column16(k,2),column17(k,2),column18(k,2)],'ro-','LineWidth',1,'MarkerEdgeColor','r','MarkerFaceColor','r');
    end
end

axis([25 60 50 85])

%%
%
clc;
clear

figure(4)
filename = 'D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_gridmap.txt';
kitinggridmap = importdata(filename);

b=bar3(kitinggridmap,1);
colorbar

for k = 1:length(b)
    zdata = b(k).ZData;
    b(k).CData = zdata;
    b(k).FaceColor = 'interp';
end

%%
%
clc;
clear

figure(5)
filename = 'D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_fitness_noneAL.txt';
kitingpremap = importdata(filename);
sz=100;
mkr='.';
for i=1:100
    scatter(i,kitingpremap(i),sz,'r',mkr);
    hold on
end

axis([0 100 0 250])


