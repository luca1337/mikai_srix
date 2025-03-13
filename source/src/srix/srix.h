/*
 * @author      Lilz <https://telegram.me/Lilz73>
 * @copyright   2020-2021 Lilz <https://telegram.me/Lilz73>
 * @license     MIKAI LICENSE
 *
 * This file is part of MIKAI.
 *
 * MIKAI is free software: you can redistribute it and/or modify
 * it under the terms of the MIKAI License, as published by
 * Lilz along with this program and available on "MIKAI Download" Telegram channel
 * <https://telegram.me/mikaidownload>.
 *
 * MIKAI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY.
 *
 * You should have received a copy of the MIKAI License along
 * with MIKAI.
 * If not, see <https://telegram.me/mikaidownload>.
 */

#ifndef MIKAI_SRIX_H
#define MIKAI_SRIX_H

#include <stdint.h>
#include <stdlib.h>
#include <mikai/mikai.h>
#include <mikai-error.h>
#include "srix-flag.h"

typedef struct Srix Srix;

/**
 * Create a new Srix and set its default values.
 * @return null if there is an error, else a Srix struct pointer
 */
Srix *SrixNew();

/**
 * Delete a Srix and free its memory.
 * @param target Srix instance to delete
 */
void SrixDelete(Srix *target);

/**
 * Function that search for available NFC readers and return their number.
 * @param target pointer to Srix struct
 * @return number of readers found
 */
size_t NfcGetReadersCount(Srix *target);

/**
 * Function that return specified nfc reader description (connection string).
 * @param target pointer to Srix struct
 * @param reader index of reader (0 = first, 1 = second, ecc.)
 * @return connstring of reader at specified index
 */
char *NfcGetDescription(Srix *target, int reader);

/**
 * Initialize the Srix using Nfc.
 * @param target pointer to Srix struct
 * @param reader index of nfc reader to use
 * @return MikaiError
 */
MikaiError SrixNfcInit(Srix *target, int reader);

/**
 * Initialize the Srix using values in memory.
 * @param target pointer to Srix struct
 * @param eeprom pointer to EEPROM array to import
 * @param uid UID to import
 */
void SrixMemoryInit(Srix *target, uint32_t eeprom[const static SRIX4K_BLOCKS], uint64_t uid);

/**
 * Return UID of an initialized srix.
 * @param target pointer to Srix struct
 * @return uid uint64 value
 */
uint64_t SrixGetUid(Srix *target);

/**
 * Get pointer to a specified block.
 * @param target pointer to Srix struct
 * @param blockNum number of block to get
 * @return pointer to blockNum block
 */
uint32_t *SrixGetBlock(Srix *target, uint8_t blockNum);

/**
 * Modify manually a Srix block and add flag automatically.
 * @param target pointer to Srix struct
 * @param block value to write to blockNum
 * @param blockNum index of block to write
 */
void SrixModifyBlock(Srix *target, uint32_t block, uint8_t blockNum);

/**
 * Write all modified blocks of target to physical SRIX4K.
 * @param target pointer to Srix struct
 * @return MikaiError
 */
MikaiError SrixWriteBlocks(Srix *target);

#endif /* MIKAI_SRIX_H */