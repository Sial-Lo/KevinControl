#include "DataHandler.h"


DataHandler_T gData = {
    .Battery = {
        .soc = 0,
        .voltage_12V = false,
        .voltage_230V = false
    }
};


DataHandler_T *DataHandler_get(void)
{
    return &gData;
}
