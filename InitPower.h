/*
 * InitPower.h
 *
 *  Created on: 04.12.2013
 *      Author: small
 */

#ifndef INITPOWER_H_
#define INITPOWER_H_

#define MHZTOHZ       (1000000)

#define CLKIN         (25  * MHZTOHZ)
#define CORE_MAX      (500 * MHZTOHZ)
#define SYS_MAX       (125 * MHZTOHZ)
#define VCO_MIN       (72  * MHZTOHZ)

void InitPower(void);

#endif /* INITPOWER_H_ */
