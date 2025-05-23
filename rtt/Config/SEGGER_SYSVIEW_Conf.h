/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2024 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.60a                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Conf.h
Purpose : SEGGER SystemView configuration file.
          Set defines which deviate from the defaults (see SEGGER_SYSVIEW_ConfDefaults.h) here.
Revision: $Rev: 21292 $

Additional information:
  Required defines which must be set are:
    SEGGER_SYSVIEW_GET_TIMESTAMP
    SEGGER_SYSVIEW_GET_INTERRUPT_ID
  For known compilers and cores, these might be set to good defaults
  in SEGGER_SYSVIEW_ConfDefaults.h.

  SystemView needs a (nestable) locking mechanism.
  If not defined, the RTT locking mechanism is used,
  which then needs to be properly configured.
*/

#ifndef SEGGER_SYSVIEW_CONF_H
#define SEGGER_SYSVIEW_CONF_H

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */

/*********************************************************************
 *
 *       SystemView buffer configuration
 */
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE 1024 // Number of bytes that SystemView uses for the buffer.
#define SEGGER_SYSVIEW_RTT_CHANNEL 1        // The RTT channel that SystemView will use. 0: Auto selection

#define SEGGER_SYSVIEW_USE_STATIC_BUFFER 1 // Use a static buffer to generate events instead of a buffer on the stack

#define SEGGER_SYSVIEW_POST_MORTEM_MODE 0 // 1: Enable post mortem analysis mode

/*********************************************************************

/*********************************************************************
*
*       SystemView timestamp configuration
*/

#define SEGGER_SYSVIEW_GET_TIMESTAMP() (*(U32 *) (0xE0001004)) // Retrieve a system timestamp. Cortex-M cycle counter.
#define SEGGER_SYSVIEW_TIMESTAMP_BITS 32

/*********************************************************************
 *
 *       SystemView Id configuration
 */
#define SEGGER_SYSVIEW_ID_BASE                                                                                         \
	0x20000000 // Default value for the lowest Id reported by the application. Can be overridden by the application via
	           // SEGGER_SYSVIEW_SetRAMBase(). (i.e. 0x20000000 when all Ids are an address in this RAM)
#define SEGGER_SYSVIEW_ID_SHIFT                                                                                        \
	2 // Number of bits to shift the Id to save bandwidth. (i.e. 2 when Ids are 4 byte aligned)

/*********************************************************************
 *
 *       SystemView interrupt configuration
 */

#define SEGGER_SYSVIEW_GET_INTERRUPT_ID()                                                                              \
	((*(U32 *) (0xE000ED04)) &                                                                                         \
	 0x1FF) // Get the currently active interrupt Id. (i.e. read Cortex-M ICSR[8:0] = active vector)

/*********************************************************************
 *
 *       Define: SEGGER_SYSVIEW_SECTION
 *
 *  Description
 *    Section to place the SystemView RTT Buffer into.
 *  Default
 *    undefined: Do not place into a specific section.
 *  Notes
 *    If SEGGER_RTT_SECTION is defined, the default changes to use
 *    this section for the SystemView RTT Buffer, too.
 */
#if !(defined SEGGER_SYSVIEW_SECTION) && (defined SEGGER_RTT_BUFFER_SECTION)
#define SEGGER_SYSVIEW_SECTION SEGGER_RTT_BUFFER_SECTION
#endif

/*********************************************************************
 * TODO: Add your defines here.                                       *
 **********************************************************************
 */

#endif // SEGGER_SYSVIEW_CONF_H

/*************************** End of file ****************************/
