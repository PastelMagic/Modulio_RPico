#include "Modulio_Target.h"

#if (RaspberryPi)
#include "Modulio_RPi.h"
#elif (RaspiPico)
#include "Modulio_RPico.h"
#else
#include "Modulio_Common.h"
#endif
