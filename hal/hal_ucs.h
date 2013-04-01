/*
 * @file hal_ucs.h
 */

#ifndef _HAL_UCS_H_
#define _HAL_UCS_H_

#define UCS_MODE_CUSTOM  -1
#define UCS_MODE_FACTORY 0
#define UCS_MODE_16M     1

void halUCS_setFrequencyDiagnosis(int mode);
int halUCS_getFrequencyMode();
void halUCS_disableFrequencyMode();

void halUCS_setFactoryFrequency();
void halUCS_set16MFrequency();

#endif
