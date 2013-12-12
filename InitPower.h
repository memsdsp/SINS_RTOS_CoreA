/*
 * InitPower.h
 *
 *  Created on: Oct 16, 2013
 *      Author: user
 */

#ifndef INITPOWER_H_
#define INITPOWER_H_

#define MHZTOHZ       (1000000)

/* default power settings */
#define DF_DEFAULT    (0x0)
#define MSEL_DEFAULT  (0x10)
#define SSEL_DEFAULT  (0x5)
#define CSEL_DEFAULT  (0x0)

#define CLKIN         (25 * MHZTOHZ)
#define CORE_MAX      (500 * MHZTOHZ)
#define SYSTEM_MAX    (125 * MHZTOHZ)
#define VCO_MIN       (50 * MHZTOHZ)

void InitPower(void);

#endif /* INITPOWER_H_ */
