close all;
clear all;

M = csvread('data_out.data');

figure()
plot(M(:,1),M(:,2))
title('Fitness');
xlabel('Iteration');
ylabel('Lowest fitness value');
