%%
%绘制游戏信息
clc;
clear;
close all;

GameInfo=importdata('D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IMptr_list.txt');
PerGameLoop=GameInfo(:,1);
GameLoop=GameInfo(:,2);
HPselfdM=GameInfo(:,3);
HPenemydM=GameInfo(:,4);

plot(HPselfdM);
hold on 
plot(HPenemydM);

title('\fontname{Times New Roman}Self/Enemy Hp/MaxHp'); 
xlabel('\fontname{Times New Roman}GameLoop','FontName','Times New Roman','FontSize',12);
ylabel('\fontname{Times New Roman}Hp/MaxHp','FontName','Times New Roman','FontSize',12);
legend('\fontname{Times New Roman}Self:Hp/MaxHp','\fontname{Times New Roman}Enemy:Hp/MaxHp');