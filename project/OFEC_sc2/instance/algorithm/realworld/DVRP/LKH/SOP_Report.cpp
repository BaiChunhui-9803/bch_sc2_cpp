#include "./INCLUDE/LKH.h"

void LKH::LKHAlg::SOP_Report(GainType Cost)
{
    printff("  Cost = " GainFormat "_" GainFormat "\n",
            CurrentPenalty, Cost);
}
