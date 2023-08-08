clc;
clear;
figure(2);
subplot(1,2,1);

cap = load('./data/msvc-bucket_count.log');
size = load('./data/msvc-factor.log');
yyaxis left
plot(cap, '-', 'LineWidth',2);
ylabel('bucket count');
hold on

yyaxis right
plot(size, '-.', 'LineWidth',2);
ylabel('load factor')
title('MSVC hashtable');
grid on
hold on


subplot(1,2,2);
cap = load('./data/gnu-bucket_count.log');
size = load('./data/gnu-factor.log');
yyaxis left
plot(cap, '-', 'LineWidth',2);
ylabel('bucket count');
hold on

yyaxis right
plot(size, '-.', 'LineWidth',2);
ylabel('load factor')
title('GNU hashtable');
grid on
hold on