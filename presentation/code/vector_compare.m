clc;
clear;
figure(2);
subplot(1,2,1);

cap = load('./data/msvc-cap.log');
size = load('./data/msvc-size.log');

plot(cap, '--', 'LineWidth',2);
hold on
plot(size, '-', 'LineWidth',2);
legend('capacity', 'size');
title('MSVC vector');
grid on
hold on


subplot(1,2,2);
cap = load('./data/gnu-cap.log');
size = load('./data/gnu-size.log');

plot(cap, '--', 'LineWidth',2);
hold on
plot(size, '-', 'LineWidth',2);
legend('capacity', 'size');
title('GNU vector');
grid on
hold on