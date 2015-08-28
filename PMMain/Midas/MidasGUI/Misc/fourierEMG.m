function fourierEMG(fileName)

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

EMG_POWER = 1;
maxEMGFiltered = power(max(emgFiltered(:,:)')', EMG_POWER);

plot(fft(myoDataVector(:,1)))
