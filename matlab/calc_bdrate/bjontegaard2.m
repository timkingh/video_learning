function bd = bjontegaard2(R1,PSNR1,R2,PSNR2, mode, result_path)

%bjontegaard2    Bjontegaard metric calculation
%   Bjontegaard's metric allows to compute the average gain in PSNR or the
%   average per cent saving in bitrate between two rate-distortion
%   curves [1]. 
%   Differently from the avsnr software package or VCEG Excel [2] plugin this
%   tool enables Bjontegaard's metric computation also with more than 4 RD
%   points.
%   Fixed integration interval in version 2.
%
%   R1,PSNR1 - RD points for curve 1
%   R2,PSNR2 - RD points for curve 2
%   mode - 
%       'dsnr' - average PSNR difference
%       'rate' - percentage of bitrate saving between data set 1 and
%                data set 2
%
%   avg_diff - the calculated Bjontegaard metric ('dsnr' or 'rate')
%   
%   (c) 2010 Giuseppe Valenzise
%
%% Bugfix 20130515
%   Original script contained error in calculation of integration interval.
%   It was fixed according to description and figure 3 in original
%   publication [1]. Script was verifyed using data presented in [3].
%   Fixed lines labeled as "(fixed 20130515)"
%
%   (c) 2013 Serge Matyunin 
%%
%
%   References:
%
%   [1] G. Bjontegaard, Calculation of average PSNR differences between
%       RD-curves (VCEG-M33)
%   [2] S. Pateux, J. Jung, An excel add-in for computing Bjontegaard metric and
%       its evolution
%   [3] VCEG-M34. http://wftp3.itu.int/av-arch/video-site/0104_Aus/VCEG-M34.xls
%
% convert rates in logarithmic units
lR1 = log(R1);	%以e为底的自然对数
lR2 = log(R2);

% figure;
% hold on
% % plot(R1, PSNR1, 'r-o');
% % plot(R2, PSNR2, 'g-o');
% % plot(R3, PSNR3, 'b-o');
% % plot(R4, PSNR4, 'm-o');
% values = spcrv([[R1(1) R1 R1(end)];[PSNR1(1) PSNR1 PSNR1(end)]],3);
% plot(values(1,:),values(2,:), 'r');
% values = spcrv([[R2(1) R2 R2(end)];[PSNR2(1) PSNR2 PSNR2(end)]],3);
% plot(values(1,:),values(2,:), 'g');
% values = spcrv([[R3(1) R3 R3(end)];[PSNR3(1) PSNR3 PSNR3(end)]],3);
% plot(values(1,:),values(2,:), 'b');
% values = spcrv([[R4(1) R4 R4(end)];[PSNR4(1) PSNR4 PSNR4(end)]],3);
% plot(values(1,:),values(2,:), 'm');

% xlabel('Kbit rate')
% ylabel('PSNR(dB)')
% legend('Hantro1', 'Rk264', 'Rk265', 'Chipmedia', 'Location','NorthEastOutside')
% grid on
% set(gca, 'xtick', 0:4000:56000);
% set(gca, 'ytick', 21:1:47);
% title('Relation between PSNR and Bitrate');

switch lower(mode)
    case 'dsnr'
        % PSNR method
        p1 = polyfit(lR1,PSNR1,2);	%X坐标是lR1，Y坐标是PSNR1，3是拟合的阶数；
        p2 = polyfit(lR2,PSNR2,2);
        
        subplot(2,1,2)
        hold on
        plot(lR1, PSNR1,  'r-*');
        plot(lR2, PSNR2,  'g-o');
        xlabel('ln(bitrate)')
        ylabel('PSNR(dB)')
        legend('TML 5.9', '4 blocks', 'Location','NorthEastOutside')
        grid on
        %set(gca, 'xtick', 0:500:2500);
        set(gca, 'ytick', 20:2:35);
        title('Relation between PSNR and ln(Bitrate)');
        
        % integration interval (fixed 20130515)
        min_int = max([ min(lR1); min(lR2) ]);
        max_int = min([ max(lR1); max(lR2) ]);
        

        % find integral
        p_int1 = polyint(p1);	%对多项式p1进行积分, 得到新的多项式
        p_int2 = polyint(p2);

		%分别计算曲线1和曲线2在[min_int,max_int]之间的面积
        int1 = polyval(p_int1, max_int) - polyval(p_int1, min_int);	%多项式p_int1在max_int处的值减去min_int处的值
        int2 = polyval(p_int2, max_int) - polyval(p_int2, min_int);

		%曲线1和曲线2在[min_int,max_int]之间的面积差除以宽度，等于高度
        % find avg diff
        avg_diff = (int2-int1)/(max_int-min_int);

    case 'rate'
        % rate method
        p1 = polyfit(PSNR1,lR1,3);
        p2 = polyfit(PSNR2,lR2,3);

%         subplot(2,1,2)
%         hold on
%         plot(PSNR1,  lR1, 'r-o');
%         plot(PSNR2,  lR2, 'g-o');
%         plot(PSNR3,  lR3, 'b-o');
%         plot(PSNR4,  lR4, 'm-o');
%         
%         ylabel('ln(Kbit rate)')
%         xlabel('PSNR(dB)')
%         legend('Hantro1', 'Rk264', 'Rk265', 'Chipmedia', 'Location','NorthEastOutside')
%         grid on
%         set(gca, 'xtick', 21:2:47);
%         title('Relation between ln(Kbit rate) and PSNR');
        
        % integration interval (fixed 20130515)
        min_int2_1 = max([ min(PSNR1); min(PSNR2) ]);
        max_int2_1 = min([ max(PSNR1); max(PSNR2) ]);

        % find integral
        p_intr1 = polyint(p1);
        p_intr2 = polyint(p2);

        intr1_2 = polyval(p_intr1, max_int2_1) - polyval(p_intr1, min_int2_1);
        intr2_1 = polyval(p_intr2, max_int2_1) - polyval(p_intr2, min_int2_1);

        fp = fopen(result_path,'ab');
        % find avg diff
        avg_exp_diff = (intr2_1-intr1_2)/(max_int2_1-min_int2_1);
        bd = (exp(avg_exp_diff))*100;
        fprintf(fp, '%1.2f\n', bd - 100);        
        fclose(fp);
end