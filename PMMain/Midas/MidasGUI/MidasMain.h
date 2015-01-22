#ifndef MIDAS_MAIN_H
#define MIDAS_MAIN_H

#include "MidasThread.h"

static MidasThread *gThreadHandle;

int midasMain(MidasThread *threadHandle, SequenceDisplayer* sequenceDisplayer);

#endif /* MIDAS_MAIN_H */