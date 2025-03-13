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

#ifndef MIKAI_READER_H
#define MIKAI_READER_H

#include <stdint.h>
#include <mikai-error.h>
#include <nfc/nfc.h>

#define MAX_DEVICE_COUNT  8
#define MAX_TARGET_COUNT  1

/**
 * Single SRIX block.
 */
typedef struct SrixBlock {
    uint8_t block[SRIX_BLOCK_LENGTH];
} SrixBlock;

/**
 * Struct that represents a NFC Reader.
 */
typedef struct NfcReader {
    nfc_connstring libnfc_readers[MAX_DEVICE_COUNT];  /* readers connstring array */
    nfc_device *libnfc_reader;                        /* libnfc reader */
} NfcReader;

/**
 * Allocate a nfc reader and set its default values.
 * @return null if there is an error, else an nfc reader pointer
 */
MIKAI_EXPORT NfcReader *NfcReaderNew();

/**
 * Close a nfc reader.
 * @param reader reader instance where reader is saved
 */
MIKAI_EXPORT void NfcCloseReader(NfcReader *reader);

/**
 * Update available readers on NfcReader instance.
 * @param reader pointer to NfcReader
 * @return number of readers currently available and saved on instance
 */
MIKAI_EXPORT size_t NfcUpdateReaders(NfcReader *reader);

/**
 * Get a description of a specific reader.
 * @param reader pointer to a NfcReader instance
 * @param selection index of reader to get
 * @return pointer to reader description string
 */
MIKAI_EXPORT char *NfcGetReaderDescription(NfcReader *reader, int selection);

/**
 * Initialize an NFC Reader.
 * @param reader pointer to Reader struct
 * @param selection id of Reader to initialize
 * @return MikaiError result
 */
MIKAI_EXPORT MikaiError NfcInitReader(NfcReader *reader, int selection);

/**
 * Get UID from Reader as raw byte array.
 * @param reader pointer to Reader struct
 * @param uid array where save the UID data
 * @return MikaiError result
 */
MIKAI_EXPORT MikaiError NfcGetUid(NfcReader *reader, uint8_t uid[const static SRIX_UID_LENGTH]);

/**
 * Read a specified block from SRIX4K to rx_data array.
 * @param reader nfc reader to send command
 * @param block array to save read block
 * @param blockNum block to read from SRIX
 * @return MikaiError result
 */
MIKAI_EXPORT MikaiError NfcReadBlock(NfcReader *reader, SrixBlock *block, uint8_t blockNum);

/**
 * Write block to SRIX4K.
 * @param reader nfc reader to send command
 * @param block array of data to write to block
 * @param blockNum block to write to SRIX
 * @return MikaiError result
 */
MIKAI_EXPORT MikaiError NfcWriteBlock(NfcReader *reader, SrixBlock *block, uint8_t blockNum);

#endif /* MIKAI_READER_H */