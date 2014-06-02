/*****************************************************************************
* Model: CommStackMgr.qm
* File:  ./CommStackMgr_gen.h
*
* This file has been generated automatically by QP Modeler (QM).
* DO NOT EDIT THIS FILE MANUALLY.
*
* Please visit www.state-machine.com/qm for more information.
*****************************************************************************/
// $Id$
/**
 * @file    CommStackMgr.h
 * Declarations for functions for the CommStackMgr AO.  This state
 * machine handles all incoming messages and their distribution for the
 * bootloader.
 *
 * Note: If editing this file, please make sure to update the CommStackMgr.qm
 * model.  The generated code from that model should be very similar to the
 * code in this file.
 *
 * @date    05/27/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 */
// $Log$
#ifndef COMM_STACK_MGR_H
#define COMM_STACK_MGR_H
#include "qp_port.h"

/* @(/2/2) .................................................................*/
/** 
* This function is a C "constructor" for this active object.
*/
void CommStackMgr_ctor(void);

extern QActive * const AO_CommStackMgr;
#endif                                                    /* COMM_STACK_MGR_H */
/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
