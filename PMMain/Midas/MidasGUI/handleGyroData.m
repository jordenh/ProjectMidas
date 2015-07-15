function handleGyroData(fileName)

%clear figures
delete(findall(0,'Type','figure'))

myoDataVector = importdata(strcat('C:\Users\jhetherington\Documents\ProjectMidas\PMMain\Midas\MidasGUI\', fileName));

gyroX = myoDataVector(:,1);
gyroY = myoDataVector(:,2);
gyroZ = myoDataVector(:,3);

x = [1:size(gyroX)];

figure
plot(x, gyroX);
title('X Gyro');
figure
plot(x, gyroY);
title('Y Gyro');
figure
plot(x, gyroZ);
title('Z Gyro');

figure
plot(x, gyroX, x, gyroY, x, gyroZ);
title('Gyro Data');

figure
ROOT = 2;
scaledGyroX = nthroot(abs(gyroX),ROOT).*sign(gyroX);
scaledGyroY = nthroot(abs(gyroY),ROOT).*sign(gyroY);
scaledGyroZ = nthroot(abs(gyroZ),ROOT).*sign(gyroZ);
plot(x, scaledGyroX, x, scaledGyroY, x, scaledGyroZ);
title('Gyro Data NthRoot');

figure
SCALE = 1000;
scaledGyroX = gyroX/SCALE;
scaledGyroY = gyroY/SCALE;
scaledGyroZ = gyroZ/SCALE;
plot(x, scaledGyroX, x, scaledGyroY, x, scaledGyroZ);
title('Gyro Data Scaled by 1000');