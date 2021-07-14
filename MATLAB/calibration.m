clc, clear
data = importdata('calibration_log.txt');
n = size(data,1);
data_filtered = [];
for i = 1:n
    data_column = data{i,1};
    find_REF = find(data_column == 'F');
    if(~isempty(find_REF))
        position = str2num(data_column(find_REF+1));
        rssi = extractNumFromStr(data_column(find_REF+2:end));
        column_filtered{1,position} = rssi;
        
        if(size(column_filtered,2)==3)
            [test1,test2,test3] = column_filtered{1,:};
            test_flag = ~isempty(test1) && ~isempty(test2) && ~isempty(test3);
            if(test_flag)
                data_filtered = [data_filtered; [column_filtered{:}]];
                column_filtered = {};
            end
        end
    end
end

RSSI_REF1 = -43.5225;
RSSI_REF2 = -26.3252;
RSSI_REF3 = -29.7935;
PATH_LOSS = 2;

d(:,1) = 10.^((data_filtered(:,1)-RSSI_REF1)/(-10*PATH_LOSS));
d(:,2) = 10.^((data_filtered(:,2)-RSSI_REF2)/(-10*PATH_LOSS));
d(:,3) = 10.^((data_filtered(:,3)-RSSI_REF3)/(-10*PATH_LOSS));

dd = 0.1;
rx = (d(:,1).^2-d(:,2).^2+dd^2)/(2*dd);
ry = (d(:,1).^2+d(:,2).^2-2*d(:,3).^2+dd^2)/(2*sqrt(3)*dd);

figure, 
plot(d(10:end,:))
title('Distancing from each fixed node to movable node');
xlabel('Iteration');
ylabel('Distance [m]');
legend('FX\_ND\_1', 'FX\_ND\_2', 'FX\_ND\_3');

% order = 50;
% mmean = ones(order,1)/order;

% for i = 1:3
%     data_filtered(:,i) = filter(mmean,1,data_filtered(:,i));
% end
% 
% figure, 
% a = num2str(mean(data_filtered(50:end,1)));
% b = num2str(mean(data_filtered(50:end,2)));
% c = num2str(mean(data_filtered(50:end,3)));



% plot(data_filtered(50:end,:),'LineWidth',2);
% text(10,-33.5,strcat('FXD\_ND\_1, mean =',a, ' dBm'))
% text(10,-35,strcat('FXD\_ND\_2, mean =',b, ' dBm'))
% text(10,-36.5,strcat('FXD\_ND\_3, mean =',c, ' dBm'))
% title('Calibration of RSSI from fixed nodes distancig 1m from movable node')
% xlabel('Cycle of reading')
% ylabel('Power [dBm]')
% legend('FXD\_ND\_1', 'FXD\_ND\_2', 'FXD\_ND\_3', 'Location', 'east');
