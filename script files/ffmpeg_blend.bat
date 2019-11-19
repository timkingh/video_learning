
goto start
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\yuv_backup\ipc_yuv\311_1080p_10000kbps_h265_300frames.yuv ^
       -vf "format=gbrp,split[a][b];[a][b]blend=all_expr='A*(sin(PI/2*min(1\,max(-1\,1/2*(T-3))))+1)/2':enable='lte(t,5)',format=yuv420p" out.yuv
       
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\yuv_backup\ipc_yuv\311_1080p_10000kbps_h265_300frames.yuv ^
       -vf "format=gbrp,split[a][b];[a][b]blend=all_expr='A*(50-N)/N':enable='lte(n,50)',format=yuv420p" -vframes 60 -y out.yuv

ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\yuv_backup\ipc_yuv\311_1080p_10000kbps_h265_300frames.yuv ^
       -vf "lutyuv=y=16:u=128:v=128" -y black_image_1080p.yuv

ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\yuv_backup\ipc_yuv\311_1080p_10000kbps_h265_300frames.yuv ^
       -vf "lutyuv=y=239:u=128:v=128" -y white_image_1080p.yuv  
       
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\yuv_backup\ipc_yuv\311_1080p_10000kbps_h265_300frames.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i black_image_1080p.yuv ^
       -filter_complex "blend=all_expr='A*(50-N)/50+B*N/50':enable='lte(n,50)'" -vframes 60 -y out.yuv   
       
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\yuv_backup\ipc_yuv\311_1080p_10000kbps_h265_300frames.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='if(between(mod(N,30),10,15),A*(100-mod(N,30))/100+B*mod(N,30)/100,A)'" -vframes 100 -y out.yuv                

:start
rem BQTerrace_1920x1080_60.yuv 601
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='A*(300-N)/300+B*N/300':enable='lte(n,300)'" ^
       -vframes %1 -y 000000_BQTerrace_1080P_linear_white_t300.yuv

ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='A*(200-N)/200+B*N/200':enable='lte(n,200)'" ^
       -vframes %1 -y 000001_BQTerrace_1080P_linear_white_t200.yuv

ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='A*(100-N)/100+B*N/100':enable='lte(n,100)'" ^
       -vframes %1 -y 000002_BQTerrace_linear_white_t100.yuv

ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='if(between(mod(N,30),10,15),A*(1000-N)/1000+B*N/1000,A)'" ^
       -vframes %1 -y 000003_BQTerrace_linear_white_t1000.yuv

rem 000004
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='if(between(mod(N,30),10,15),A*(1000-mod(N,30))/1000+B*mod(N,30)/1000,A)'" ^
       -vframes %1 -y 000004_BQTerrace_linear_white_t1000.yuv

rem vs. 000004
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i black_image_1080p.yuv ^
       -filter_complex "blend=all_expr='if(between(mod(N,30),10,15),A*(1000-mod(N,30))/1000+B*mod(N,30)/1000,A)'" ^
       -vframes %1 -y 000105_BQTerrace_linear_black_t1000.yuv

rem vs. 000001
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -filter_complex "blend=all_expr='if(lt(N,200),A*(200-N)/200+B*N/200,B)'" ^
       -vframes %1 -y 000006_BQTerrace_1080P_linear_white_t200.yuv 

rem quadratic vs. 000002
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='A*(1-N*N/10000)+B*N*N/10000':enable='lte(n,100)'" ^
       -vframes %1 -y 001007_BQTerrace_quadratic_white_t100.yuv

rem S-curve vs. 000002
ffmpeg -pix_fmt yuv420p -s 1920x1080 -i F:\rkvenc_verify\input_yuv\yuv\BQTerrace_1920x1080_60.yuv ^
       -pix_fmt yuv420p -s 1920x1080 -i white_image_1080p.yuv ^
       -filter_complex "blend=all_expr='if(lt(N,50),A*(1-2*N*N/10000)+B*2*N*N/10000,A*2*(100-N)*(100-N)/10000+B*(1-2*(100-N)*(100-N)/10000))':enable='lte(n,100)'" ^
       -vframes %1 -y 002008_BQTerrace_S_curve_white_t100.yuv



