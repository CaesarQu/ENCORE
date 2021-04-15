clear all; close all; clc;
format long;

fsize=55;
fsize_legend = 15;
fsize_axis = 40;
fname = 'Times New Roman';

%%
% First 5 columns are GEAR with lambda = 0,0.25,0.5,0.75,1
% Next 5 columns are SPIDER with lambda = 0,0.25,0.5,0.75,1
% Next column is AODV
% Last column is HWMP

Low_Failure_Energy = xlsread('SPIDER.xlsx',1);
High_Failure_Energy = xlsread('SPIDER.xlsx',2);
Low_Mobility_Energy = xlsread('SPIDER.xlsx',3);
High_Mobility_Energy = xlsread('SPIDER.xlsx',4);
Low_Failure_Throughput = xlsread('SPIDER.xlsx',5);
High_Failure_Throughput = xlsread('SPIDER.xlsx',6);
Low_Mobility_Throughput = xlsread('SPIDER.xlsx',7);
High_Mobility_Throughput = xlsread('SPIDER.xlsx',8);

%% Throughput ploting
for i=1:12
    %Failure cases
    %mu_low_failure(i)=mean(Low_Failure_Throughput(2:end,i));
    sigma_low_failure(i)=std(Low_Failure_Throughput(2:end,i));
    [mu_low_failure(i),ci_low_failure(i)]=avg_ci(Low_Failure_Throughput(2:end,i));
    %mu_high_failure(i)=mean(High_Failure_Throughput(2:end,i));
    sigma_high_failure(i)=std(High_Failure_Throughput(2:end,i));
    [mu_high_failure(i),ci_high_failure(i)]=avg_ci(High_Failure_Throughput(2:end,i));
    %Mobility cases
    %mu_low_mobility(i)=mean(Low_Mobility_Throughput(2:end,i));
    sigma_low_mobility(i)=std(Low_Mobility_Throughput(2:end,i));
    [mu_low_mobility(i),ci_low_mobility(i)]=avg_ci(Low_Mobility_Throughput(2:end,i));
    %mu_high_mobility(i)=mean(High_Mobility_Throughput(2:end,i));
    sigma_high_mobility(i)=std(High_Mobility_Throughput(2:end,i));
    [mu_high_mobility(i),ci_high_mobility(i)]=avg_ci(High_Mobility_Throughput(2:end,i));
end

%% Low failure scnearios
fig=figure;
LF = [mu_low_failure(:,1:5);
     mu_low_failure(:,6:10)];
LF_sig = [sigma_low_failure(:,1:5);
     sigma_low_failure(:,6:10)]; 
LF_ci = [ci_low_failure(:,1:5);
     ci_low_failure(:,6:10)];
hB=bar(LF,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
end
errorbar(ctr, ydt, LF_ci', '.k');
bar([3],[mu_low_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_low_failure(:,11),ci_low_failure(:,11),'.k');
bar([4],[mu_low_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_low_failure(:,12),ci_low_failure(:,12),'.k');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Throughput [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 5]);
% title('Low Failure QoS');
colormap(gray);
% figHandle=legend([hB],'\lambda = 0   ', '\lambda = 0.25   ', '\lambda = 0.50   ', '\lambda = 0.75   ', '\lambda = 1   ');
% set(legend,'Location','northoutside'); 
% set(figHandle,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
% set(figHandle,'fontname',fname);
% set(figHandle,'fontsize',fsize_legend);
% set(figHandle,'position',[0 0 1 1])
% set(figHandle,'Location','northoutside');
% set(figHandle,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
% saveLegendToImage(fig, figHandle, 'legend', 'eps');

% plot std values
figure;
hB=bar(LF_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_low_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_low_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 3]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% High failure scnearios
figure;
HF = [mu_high_failure(:,1:5);
     mu_high_failure(:,6:10)];
HF_sig = [sigma_high_failure(:,1:5);
     sigma_high_failure(:,6:10)]; 
HF_ci = [ci_high_failure(:,1:5);
     ci_high_failure(:,6:10)];
hB=bar(HF,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
end
errorbar(ctr, ydt, HF_ci', '.k')
bar([3],[mu_high_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_high_failure(:,11),ci_high_failure(:,11),'.k');
bar([4],[mu_high_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_high_failure(:,12),ci_high_failure(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Throughput [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 5]);
% title('High Failure QoS');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%plot std values
figure;
hB=bar(HF_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_high_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_high_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 3]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% Low mobility scnearios
figure;
LM = [mu_low_mobility(:,1:5);
     mu_low_mobility(:,6:10)];
LM_sig = [sigma_low_mobility(:,1:5);
     sigma_low_mobility(:,6:10)];
LM_ci = [ci_low_mobility(:,1:5);
     ci_low_mobility(:,6:10)];
hB=bar(LM,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
end
errorbar(ctr, ydt, LM_ci', '.k')
bar([3],[mu_low_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_low_mobility(:,11),ci_low_mobility(:,11),'.k');
bar([4],[mu_low_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_low_mobility(:,12),ci_low_mobility(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Throughput [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 5]);
% title('Low Mobility QoS');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%plot std values
figure;
hB=bar(LM_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_low_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_low_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 3]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% High mobility scnearios
figure;
HM = [mu_high_mobility(:,1:5);
     mu_high_mobility(:,6:10)];
HM_sig = [sigma_high_mobility(:,1:5);
     sigma_high_mobility(:,6:10)];
HM_ci = [ci_high_mobility(:,1:5);
     ci_high_mobility(:,6:10)];
hB=bar(HM,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
end
errorbar(ctr, ydt, HM_ci', '.k')
bar([3],[mu_high_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_high_mobility(:,11),ci_high_mobility(:,11),'.k');
bar([4],[mu_high_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_high_mobility(:,12),ci_high_mobility(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Throughput [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 5]);
% title('High Mobility QoS');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%plot std values
figure;
hB=bar(HM_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_high_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_high_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [Mbps]','fontname','Times New Roman','fontsize',fsize);
ylim([0 3]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% caculate energy remaining
for i=1:12
    %Failure cases
    %mu_low_failure(i)=mean(Low_Failure_Energy(1:end-1,i));
    sigma_low_failure(i)=std(Low_Failure_Energy(1:end-1,i));
    [mu_low_failure(i),ci_low_failure(i)]=avg_ci(Low_Failure_Energy(1:end-1,i));
    %mu_high_failure(i)=mean(High_Failure_Energy(1:end-1,i));
    sigma_high_failure(i)=std(High_Failure_Energy(1:end-1,i));
    [mu_high_failure(i),ci_high_failure(i)]=avg_ci(High_Failure_Energy(1:end-1,i));
    %Mobility cases
    %mu_low_mobility(i)=mean(Low_Mobility_Energy(1:end-1,i));
    sigma_low_mobility(i)=std(Low_Mobility_Energy(1:end-1,i));
    [mu_low_mobility(i),ci_low_mobility(i)]=avg_ci(Low_Mobility_Energy(1:end-1,i));
    %mu_high_mobility(i)=mean(High_Mobility_Energy(1:end-1,i));
    sigma_high_mobility(i)=std(High_Mobility_Energy(1:end-1,i));
    [mu_high_mobility(i),ci_high_mobility(i)]=avg_ci(High_Mobility_Energy(1:end-1,i));
end

%% Low failure scnearios
figure;
LF = [mu_low_failure(:,1:5);
     mu_low_failure(:,6:10)];
LF_sig = [sigma_low_failure(:,1:5);
     sigma_low_failure(:,6:10)];   
LF_ci = [ci_low_failure(:,1:5);
     ci_low_failure(:,6:10)];
hB=bar(LF,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
labels = {'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1','\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'};
hT=[];              % placeholder for text object handles
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
%   hT=[hT text(hB(i).XData+hB(i).XOffset,hB(i).YData+80,labels(:,i), 'VerticalAlignment','bottom','horizontalalign','center')];
end
errorbar(ctr, ydt, LF_ci', '.k')
bar([3],[mu_low_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_low_failure(:,11),ci_low_failure(:,11),'.k');
bar([4],[mu_low_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_low_failure(:,12),ci_low_failure(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Residual Energy [J]','fontname','Times New Roman','fontsize',fsize);
ylim([400 600]);
% title('Low Failure Energy');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%plot std values
figure;
hB=bar(LF_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_low_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_low_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [J]','fontname','Times New Roman','fontsize',fsize);
% ylim([400 600]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% High failure scnearios
figure;
HF = [mu_high_failure(:,1:5);
     mu_high_failure(:,6:10)];
HF_sig = [sigma_high_failure(:,1:5);
     sigma_high_failure(:,6:10)];  
HF_ci = [ci_high_failure(:,1:5);
     ci_high_failure(:,6:10)];
hB=bar(HF,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;

labels = {'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1','\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'};
hT=[];              % placeholder for text object handles
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
%   hT=[hT text(hB(i).XData+hB(i).XOffset,hB(i).YData+80,labels(:,i), 'VerticalAlignment','bottom','horizontalalign','center')];
end
errorbar(ctr, ydt, HF_ci', '.k')
bar([3],[mu_high_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_high_failure(:,11),ci_high_failure(:,11),'.k');
bar([4],[mu_high_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_high_failure(:,12),ci_high_failure(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Residual Energy [J]','fontname','Times New Roman','fontsize',fsize);
ylim([400 600]);
% title('High Failure Energy');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

% plot std values
figure;
hB=bar(HF_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_high_failure(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_high_failure(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [J]','fontname','Times New Roman','fontsize',fsize);
% ylim([400 600]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% Low mobility scnearios
figure;
LM = [mu_low_mobility(:,1:5);
     mu_low_mobility(:,6:10)];
LM_sig = [sigma_low_mobility(:,1:5);
     sigma_low_mobility(:,6:10)];  
LM_ci = [ci_low_mobility(:,1:5);
     ci_low_mobility(:,6:10)];
hB=bar(LM,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;

labels = {'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1','\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'};
hT=[];              % placeholder for text object handles
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
%   hT=[hT text(hB(i).XData+hB(i).XOffset,hB(i).YData+80,labels(:,i), 'VerticalAlignment','bottom','horizontalalign','center')];
end
errorbar(ctr, ydt, LM_ci', '.k')
bar([3],[mu_low_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_low_mobility(:,11),ci_low_mobility(:,11),'.k');
bar([4],[mu_low_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_low_mobility(:,12),ci_low_mobility(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Residual Energy [J]','fontname','Times New Roman','fontsize',fsize);
ylim([400 600]);
% title('Low Mobility Energy');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%plot std values
figure;
hB=bar(LM_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_low_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_low_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [J]','fontname','Times New Roman','fontsize',fsize);
% ylim([400 600]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%% High mobility scnearios
figure;
HM = [mu_high_mobility(:,1:5);
     mu_high_mobility(:,6:10)];
HM_sig = [sigma_high_mobility(:,1:5);
     sigma_high_mobility(:,6:10)];
HM_ci = [ci_high_mobility(:,1:5);
     ci_high_mobility(:,6:10)];
hB=bar(HM,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;

labels = {'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1','\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'};
hT=[];              % placeholder for text object handles
for i=1:length(hB)  % iterate over number of bar objects
   ctr(i,:) = bsxfun(@plus, hB(1).XData, [hB(i).XOffset]');
   ydt(i,:) = hB(i).YData;
%   hT=[hT text(hB(i).XData+hB(i).XOffset,hB(i).YData+80,labels(:,i), 'VerticalAlignment','bottom','horizontalalign','center')];
end
errorbar(ctr, ydt, HM_ci', '.k')
bar([3],[mu_high_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
errorbar([3],mu_high_mobility(:,11),ci_high_mobility(:,11),'.k');
bar([4],[mu_high_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
errorbar([4],mu_high_mobility(:,12),ci_high_mobility(:,12),'.k');
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Residual Energy [J]','fontname','Times New Roman','fontsize',fsize);
ylim([400 600]);
% title('High Mobility Energy');
colormap(gray);
% legend({'\lambda=0', '\lambda=0.25', '\lambda=0.50', '\lambda=0.75', '\lambda=1'});
set(legend,'Location','northoutside');
set(legend,'Orientation','horizontal');
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);

%plot std values
figure;
hB=bar(HM_sig,'LineWidth',1.5); % use a meaningful variable for a handle array...
hold on;
bar([3],[sigma_high_mobility(:,11)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle',':');
bar([4],[sigma_high_mobility(:,12)],0.3,'LineWidth',1.5,'FaceColor','w','LineStyle','--');
str ={'GEAR';'SPIDER';'AODV';'HWMP'};
set(gca, 'XTickLabel',str, 'XTick',1:numel(str), 'fontname','Times New Roman','fontsize',fsize);
ylabel('Standard Deviation [J]','fontname','Times New Roman','fontsize',fsize);
% ylim([400 600]);
colormap(gray);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);
set(gca,'fontname',fname);
set(gca,'fontsize',fsize_axis);