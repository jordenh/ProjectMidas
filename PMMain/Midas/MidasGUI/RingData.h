#ifdef BUILD_KEYBOARD
#ifndef RING_DATA_H
#define RING_DATA_H

#include <vector>

class ringData
{
public:
    ringData();
    ~ringData();

    ringData(const ringData& rd);

    struct keyboardValue
    {
        char main, hold;
        keyboardValue(char mainVal, char holdVal = '\0') : main(mainVal), hold(holdVal) { }
    };


    std::vector<keyboardValue>* getRingInVectorHandle();
    std::vector<keyboardValue>* getRingOutVectorHandle();

    void setRingInVector(std::vector<keyboardValue> ringInVal);
    void setRingOutVector(std::vector<keyboardValue> ringOutVal);



private:

    std::vector<keyboardValue> *ringInVector;
    std::vector<keyboardValue> *ringOutVector;


};

#endif RING_DATA_H

#endif