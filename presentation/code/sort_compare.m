clc;
clear;

figure(2);
subplot(1,2,1);

q_log = load('./data/msvc-qsort-log.log');
std_log = load('./data/msvc-log.log');
y = [q_log';std_log']';
x = [1000, 100000, 1000000, 10000000];
bar(y);
set(gca,'XTickLabel',x);
ylabel('time(ms)');
legend('quick sort', 'std::sort');
title('MSVC sort');
grid on
hold on

subplot(1,2,2);
q_log = load('./data/gnu-qsort-log.log');
std_log = load('./data/gnu-log.log');
y = [q_log';std_log']';
x = [1000, 100000, 1000000, 10000000];
bar(y);
set(gca,'XTickLabel',x);
ylabel('time(ms)');
legend('quick sort', 'std::sort');
title('GNU sort');
grid on