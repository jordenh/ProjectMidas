function plotFromFile(fileName)

%clear figures
delete(findall(0,'Type','figure'))

%1-8 are EMG signals ranging from 0 to 255
POSE_IDX = 9;

myoDataVector = importdata(strcat('C:\Users\jhetherington\Documents\ProjectMidas\PMMain\Midas\MidasGUI\', fileName));
myoPoseValues = myoDataVector(:,POSE_IDX);

%filter each individual channel as a running average to smooth it a bit
FILTER_LEN = 5;
absEMG = abs(myoDataVector(:,1:8));
ABS_MAX = 128;
emgFiltered = filter(ones(1,FILTER_LEN), FILTER_LEN, absEMG/ABS_MAX);

maxEMGFiltered = max(emgFiltered(:,:)')';


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
MAX_EMG_IMPULSE_THRESHOLD_HIGH = 0.2;
MAX_EMG_IMPULSE_THRESHOLD_LOW = 0.05;
IMPULSE_SIZE = 40; % attempt to place a notch around IMPULSE_SIZE samples when there is an impulse
impulseCount = 0;
currImpulse = false;
impulseVec = [];
for i=1:size(maxEMGFiltered(:,1))
    if (currImpulse == false && maxEMGFiltered(i,1) > MAX_EMG_IMPULSE_THRESHOLD_HIGH)
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

% HANDLING RISING_EDGE FALLING_EDGE IMPULSE FINDING
MAX_EMG_IMPULSE_THRESHOLD = 0.18;
FALL_DETECTION_COUNT = 10;
fallDetection = FALL_DETECTION_COUNT;
IMPULSE_SIZE = 40; % attempt to place a notch around IMPULSE_SIZE samples when there is an impulse
impulseCount = 0;
currImpulse = false;
impulseVec = [];
for i=1:size(maxEMGFiltered(:,1))
    currentlyPosing = (myoPoseValues(i) ~= 0);
    
    if (maxEMGFiltered(i,1) <= MAX_EMG_IMPULSE_THRESHOLD)
       fallDetection = fallDetection - 1; 
    else
        fallDetection = FALL_DETECTION_COUNT;
    end
    
    if (currentlyPosing && fallDetection <= 0)
        currImpulse = true;
        impulseCount = IMPULSE_SIZE;
    end
    
    if (currImpulse && impulseCount > 0)
        impulseVec = [impulseVec, 1];
        impulseCount = impulseCount - 1;
    else
        impulseVec = [impulseVec, 0]; 
    end
    
    if (impulseCount <= 0)
        currImpulse = false;
    end
end

figure
hold on
plot(x, myoPoseValues);
plot (x, maxEMGFiltered, 'gs','LineWidth',1,'MarkerSize',10);
plot (x, impulseVec, 'r');
hold off
% END HANDLING FALLING_EDGE IMPULSE FINDING

%plot(x, myoDataVector(:,POSE_IDX));

%hold off