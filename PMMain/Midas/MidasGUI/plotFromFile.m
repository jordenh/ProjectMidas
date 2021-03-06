function plotFromFile(fileName)

%clear figures
delete(findall(0,'Type','figure'))

%1-8 are EMG signals ranging from 0 to 255
POSE_IDX = 9;
RECORDED_IMPULSE_IDX = 10;

myoDataVector = importdata(strcat('C:\Users\jhetherington\Documents\ProjectMidas\PMMain\Midas\MidasGUI\', fileName));
myoPoseValues = myoDataVector(:,POSE_IDX);
if (size(myoDataVector, 2) >= 10)
    midasImpulseValues = myoDataVector(:,RECORDED_IMPULSE_IDX);
end

%filter each individual channel as a running average to smooth it a bit
FILTER_LEN = 5;
absEMG = abs(myoDataVector(:,1:8));
ABS_MAX = 128;
emgFiltered = filter(ones(1,FILTER_LEN), FILTER_LEN, absEMG/ABS_MAX);

EMG_POWER = 3;
maxEMGFiltered = power(max(emgFiltered(:,:)')', EMG_POWER);

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
plot (x, maxEMGFiltered, 'g');
hold off


% HANDLING RISING_EDGE IMPULSE FINDING
MAX_EMG_IMPULSE_THRESHOLD_HIGH = 0.125; % Midas currently at 0.2
MAX_EMG_IMPULSE_THRESHOLD_LOW = 0.01;
IMPULSE_SIZE = 30; % attempt to place a notch around IMPULSE_SIZE samples when there is an impulse
impulseCount = 0;
currImpulse = false;
impulseVec = [];
for i=1:size(maxEMGFiltered(:,1))
    currentlyPosing = (myoPoseValues(i) ~= 0);
    if (currImpulse == false && maxEMGFiltered(i,1) > MAX_EMG_IMPULSE_THRESHOLD_HIGH && currentlyPosing == 0)
        currImpulse = true;
        impulseCount = IMPULSE_SIZE;
    end 
    
    if (currImpulse && impulseCount > 0)
        impulseVec = [impulseVec, 1];
        impulseCount = impulseCount - 1;
    else
        impulseVec = [impulseVec, 0]; 
    end
    
    if (impulseCount <= 0 && maxEMGFiltered(i,1) <= MAX_EMG_IMPULSE_THRESHOLD_LOW)
        currImpulse = false;
    end
end

figure
hold on
plot(x, myoPoseValues);
plot (x, maxEMGFiltered, 'g');
plot (x, impulseVec, 'r');
hold off
% END HANDLING IMPULSE FINDING

% HANDLING FALLING_EDGE IMPULSE FINDING
FALLING_EMG_IMPULSE_THRESHOLD_LOW = 0.1;
FALLING_EMG_IMPULSE_THRESHOLD_HIGH = 0.20; 
DETECTION_COUNT = 5;
fallDetection = DETECTION_COUNT;
riseDetection = DETECTION_COUNT;
musclesActive = false;
impulseCount = 0;
fallImpulseVec = [];
for i=1:size(maxEMGFiltered(:,1))
    if (maxEMGFiltered(i,1) >= FALLING_EMG_IMPULSE_THRESHOLD_HIGH)
        riseDetection = riseDetection - 1;
    else
        riseDetection = DETECTION_COUNT;
    end
    if (riseDetection <= 0)
       musclesActive = true; 
    end
    
    if (maxEMGFiltered(i,1) <= FALLING_EMG_IMPULSE_THRESHOLD_LOW)
       fallDetection = fallDetection - 1; 
    else
        fallDetection = DETECTION_COUNT;
    end
    
    if (musclesActive == true && fallDetection <= 0)
        musclesActive = false;
        impulseCount = IMPULSE_SIZE;
    end
    
    if (musclesActive == false && impulseCount > 0)
        % impulse when muscles are inactive but there is more 'count' left
        % to output
        fallImpulseVec = [fallImpulseVec, 1];
        impulseCount = impulseCount - 1;
    else
        fallImpulseVec = [fallImpulseVec, 0]; 
    end
end

figure
hold on
plot(x, myoPoseValues);
plot (x, maxEMGFiltered, 'gx','LineWidth',1,'MarkerSize',10);
plot (x, impulseVec, 'r');
plot (x, fallImpulseVec, 'k');
hold off
% END HANDLING FALLING_EDGE IMPULSE FINDING

if (size(myoDataVector, 2) >= 10)
figure
hold on
title('Midas Impulse Values - red');
plot(x, myoPoseValues);
plot (x, maxEMGFiltered, 'g');
plot (x, midasImpulseValues, 'r');
hold off
end

if (size(myoDataVector, 2) >= 10)
figure
hold on
title('Midas Impulse Values - red');
plot(x, myoPoseValues);
plot (x, maxEMGFiltered, 'g');
plot (x, midasImpulseValues, 'r');

plot (x, impulseVec, 'y');
plot (x, fallImpulseVec, 'y');
hold off
end

% DOESNT REALLY WORK...
% HANDLING RISING_EDGE FALLING_EDGE IMPULSE FINDING
%MAX_EMG_IMPULSE_THRESHOLD = 0.01;
%FALL_DETECTION_COUNT = 10;
%fallDetection = FALL_DETECTION_COUNT;
%IMPULSE_SIZE = 40; % attempt to place a notch around IMPULSE_SIZE samples when there is an impulse
%impulseCount = 0;
%currImpulse = false;
%impulseVec = [];
%for i=1:size(maxEMGFiltered(:,1))
%    currentlyPosing = (myoPoseValues(i) ~= 0);
%    
%    if (maxEMGFiltered(i,1) <= MAX_EMG_IMPULSE_THRESHOLD)
%       fallDetection = fallDetection - 1; 
%    else
%        fallDetection = FALL_DETECTION_COUNT;
%    end
%    
%    if (currentlyPosing && fallDetection <= 0)
%        currImpulse = true;
%        impulseCount = IMPULSE_SIZE;
%    end
%    
%    if (currImpulse && impulseCount > 0)
%        impulseVec = [impulseVec, 1];
%        impulseCount = impulseCount - 1;
%    else
%        impulseVec = [impulseVec, 0]; 
%    end
%    
%    if (impulseCount <= 0)
%        currImpulse = false;
%    end
%end
%
%figure
%hold on
%plot(x, myoPoseValues);
%plot (x, maxEMGFiltered, 'gx','LineWidth',1,'MarkerSize',10);
%plot (x, impulseVec, 'r');
%hold off
% END HANDLING FALLING_EDGE IMPULSE FINDING

%plot(x, myoDataVector(:,POSE_IDX));

%hold off