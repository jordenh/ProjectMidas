function plotFromFile()

%1-8 are EMG signals from 0 to 255

POSE_IDX = 9;

myoDataVector = importdata('C:\Users\jhetherington\Documents\ProjectMidas\PMMain\Midas\MidasGUI\myoDataFile.csv');
myoPoseValues = myoDataVector(:,POSE_IDX);

FILTER_LEN = 5;
absEMG = abs(myoDataVector(:,1:8));
ABS_MAX = 128;
emgFiltered = filter(ones(1,FILTER_LEN), FILTER_LEN, absEMG/ABS_MAX);



%avgEMGData = mean(abs(myoDataVector(:,1:8)' / 255)); // not bad, but too
%course.

figure
x = [1:size(myoDataVector(:,1))];
for i=1:4
   subplot(2,2,i)
   hold on
   plot(x, myoPoseValues);
   plot(x, emgFiltered(:,i), 'g');
   hold off
end
figure
for i=5:8
   subplot(2,2,i-4)
   hold on
   plot(x, myoPoseValues);
   plot(x, emgFiltered(:,i), 'g');
   hold off
end

figure
hold on
plot(x, myoPoseValues);
plot (x, max(emgFiltered(:,:)')', 'g');
hold off

%plot(x, myoDataVector(:,POSE_IDX));

%hold off