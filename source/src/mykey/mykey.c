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

#include <stdint.h>
#include <string.h>
#include <mikai/mikai.h>
#include <mikai-internal.h>
#include <mikai-error.h>
#include <srix.h>

/**
 * Encode or decode a MyKey block
 * @param block block pointer.
 */
static inline void encodeDecodeBlock(uint32_t *block) {
    /*
     * Swap all values using XOR
     * 32 bit: 1111222233334444
     */
    *block ^= (*block & 0x00C00000) << 6 | (*block & 0x0000C000) << 12 | (*block & 0x000000C0) << 18 |
              (*block & 0x000C0000) >> 6 | (*block & 0x00030000) >> 12 | (*block & 0x00000300) >> 6;
    *block ^= (*block & 0x30000000) >> 6 | (*block & 0x0C000000) >> 12 | (*block & 0x03000000) >> 18 |
              (*block & 0x00003000) << 6 | (*block & 0x00000030) << 12 | (*block & 0x0000000C) << 6;
    *block ^= (*block & 0x00C00000) << 6 | (*block & 0x0000C000) << 12 | (*block & 0x000000C0) << 18 |
              (*block & 0x000C0000) >> 6 | (*block & 0x00030000) >> 12 | (*block & 0x00000300) >> 6;
}

/**
 * Return a number between 0 and 7 that represent current transaction location.
 * @param key pointer to MyKey struct.
 * @return transaction pointer.
 */
static uint8_t getCurrentTransactionOffset(MyKey *key) {
    uint32_t *block3C = SrixGetBlock(key->srix4k, 0x3C);

    /* If first transaction, set the pointer to 7 to fill the first transaction block */
    if (*block3C == 0xFFFFFFFF) {
        return 0x07;
    }

    /* Decode transaction pointer */
    uint32_t current = *block3C ^ (*SrixGetBlock(key->srix4k, 0x07) & 0x00FFFFFF);
    encodeDecodeBlock(&current);

    if ((current & 0x00FF0000 >> 16) > 0x07) {
        /* Out of range */
        return 0x07;
    } else {
        /* Return result (a value between 0x00 and 0x07) */
        return current >> 16;
    }
}

/**
 * Calculate checksum of a generic block.
 * @param block pointer to block to checksum.
 * @param blockNum number of the block (0-127).
 */
static inline void calculateBlockChecksum(uint32_t *block, const uint8_t blockNum) {
    uint8_t checksum = 0xFF - blockNum - (*block & 0x0F) - (*block >> 4 & 0x0F) - (*block >> 8 & 0x0F) -
                       (*block >> 12 & 0x0F) - (*block >> 16 & 0x0F) - (*block >> 20 & 0x0F);

    // Clear first byte and set to checksum value
    *block &= 0x00FFFFFF;
    *block |= checksum << 24;
}

/**
 * Return the number of days between 1/1/1995 and a specified date
 * @param day day of the second date
 * @param month month of the second date
 * @param year year of the second date
 * @return difference in days
 */
static uint32_t daysDifference(uint8_t day, uint8_t month, uint16_t year) {
    if (month < 3) {
        year--;
        month += 12;
    }
    return year * 365 + year / 4 - year / 100 + year / 400 + (month * 153 + 3) / 5 + day - 728692;
}

/**
 * Calculate the encryption key and save the result in mikai struct.
 * @param key pointer to mikai data struct
 */
void calculateEncryptionKey(MyKey key[static 1]) {
    /* OTP calculation (reverse block 6 + 1, incremental. 1,2,3, ecc.) */
    uint32_t *block6 = SrixGetBlock(key->srix4k, 0x06);
    uint32_t otp = ~(*block6 << 24 | (*block6 & 0x0000FF00) << 8 |
                     (*block6 & 0x00FF0000) >> 8 | *block6 >> 24) + 1;

    /*
     * Encryption key calculation.
     * MK = UID * VENDOR
     * SK (Encryption key) = MK * OTP
     */
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x18));
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x19));

    uint32_t vendor = (*SrixGetBlock(key->srix4k, 0x18) << 16 |
                      (*SrixGetBlock(key->srix4k, 0x19) & 0x0000FFFF)) + 1;

    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x18));
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x19));

    key->encryptionKey = SrixGetUid(key->srix4k) * vendor * otp;
}

uint32_t MyKeyGetEncryptionKey(MyKey key[static 1]) {
    return key->encryptionKey;
}

bool MyKeyIsReset(MyKey *key) {
    static const uint32_t block18Reset = 0x8FCD0F48;
    static const uint32_t block19Reset = 0xC0820007;
    return *SrixGetBlock(key->srix4k, 0x18) == block18Reset &&
           *SrixGetBlock(key->srix4k, 0x19) == block19Reset;
}

bool MyKeyCheckLockID(MyKey key[static 1]) {
    /*
     * If there is lock id but block 21 checksum is right, it doesn't block mikai,
     * because key could be associated with an old reader that doesn't check lock id
     */
    uint32_t creditCheck = *SrixGetBlock(key->srix4k, 0x21) ^ key->encryptionKey;
    encodeDecodeBlock(&creditCheck);

    /* Save current checksum */
    uint8_t checksum = creditCheck >> 24;

    /* Recalculate checksum */
    calculateBlockChecksum(&creditCheck, 0x21);

    /* Check lock id and checksum */
    return (*SrixGetBlock(key->srix4k, 0x05) & 0x000000FF) == 0x7F && checksum != creditCheck >> 24;
}

uint32_t MyKeyGetBlock(MyKey key[static 1], uint8_t blockNum) {
    uint32_t *block = SrixGetBlock(key->srix4k, blockNum);
    return block ? *block : 0;
}

void MyKeyModifyBlock(MyKey key[static 1], uint32_t block, uint8_t blockNum) {
    SrixModifyBlock(key->srix4k, block, blockNum);
}

void MyKeyImportVendor(MyKey key[static 1], const uint32_t vendor) {
    /* Decode blocks 21 and 25 with precedent vendor's encryption key */
    *SrixGetBlock(key->srix4k, 0x21) ^= key->encryptionKey;
    *SrixGetBlock(key->srix4k, 0x25) ^= key->encryptionKey;

    /* Set new vendor blocks */
    uint32_t block18 = vendor >> 16;
    calculateBlockChecksum(&block18, 0x18);
    encodeDecodeBlock(&block18);
    SrixModifyBlock(key->srix4k, block18, 0x18);

    uint32_t block19 = vendor & 0x0000FFFF;
    calculateBlockChecksum(&block19, 0x19);
    encodeDecodeBlock(&block19);
    SrixModifyBlock(key->srix4k, block19, 0x19);

    /* Recalculate encryption key using new vendor */
    calculateEncryptionKey(key);

    /* Encode 21 and 25 with new vendor's encryption key */
    SrixModifyBlock(key->srix4k, *SrixGetBlock(key->srix4k, 0x21) ^ key->encryptionKey, 0x21);
    SrixModifyBlock(key->srix4k, *SrixGetBlock(key->srix4k, 0x25) ^ key->encryptionKey, 0x25);

    /* Copy vendor blocks 18 and 19 to 1C and 1D */
    SrixModifyBlock(key->srix4k, block18, 0x1C);
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x1C));
    calculateBlockChecksum(SrixGetBlock(key->srix4k, 0x1C), 0x1C);
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x1C));

    SrixModifyBlock(key->srix4k, block19, 0x1D);
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x1D));
    calculateBlockChecksum(SrixGetBlock(key->srix4k, 0x1D), 0x1D);
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x1D));
}

int MyKeyExportVendor(MyKey key[static 1], uint32_t vendor[static 1]) {
    if (MyKeyIsReset(key)) {
        key->error = MIKAI_ERROR(MIKAI_MYKEY_ERROR, "unable to export vendor, key is reset");
        return MIKAI_MYKEY_ERROR;
    }

    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x18));
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x19));

    *vendor = *SrixGetBlock(key->srix4k, 0x18) << 16 |
              *SrixGetBlock(key->srix4k, 0x19) & 0x0000FFFF;

    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x18));
    encodeDecodeBlock(SrixGetBlock(key->srix4k, 0x19));

    return MIKAI_SUCCESS;
}

void MyKeyExportMemory(MyKey key[static 1], uint32_t dump[const SRIX4K_BLOCKS], uint64_t *uid) {
    if (uid) {
        *uid = SrixGetUid(key->srix4k);
    }

    memcpy(dump, SrixGetBlock(key->srix4k, 0x00), SRIX4K_BYTES);
}

void MyKeyReset(MyKey key[static 1]) {
    for (uint8_t i = 0x10; i < SRIX4K_BLOCKS; i++) {
        uint32_t currentBlock;

        switch (i) {
            case 0x10:
            case 0x14:
            case 0x3F:
            case 0x43: {
                /* Key ID (first byte) + days elapsed from production */
                /* CHECK | ID | DAYS | DAYS */
                uint32_t *productionDate = SrixGetBlock(key->srix4k, 0x08);

                /* Decode BCD (Binary Coded Decimal) production date */
                uint8_t day = (*productionDate >> 28 & 0x0F) * 10 + (*productionDate >> 24 & 0x0F);
                uint8_t month = (*productionDate >> 20 & 0x0F) * 10 + (*productionDate >> 16 & 0x0F);
                uint16_t year = (*productionDate & 0x0F) * 1000 +
                                (*productionDate >> 4 & 0x0F) * 100 +
                                (*productionDate >> 12 & 0x0F) * 10 +
                                (*productionDate >> 8 & 0x0F);

                uint32_t elapsed = daysDifference(day, month, year);
                currentBlock = (*SrixGetBlock(key->srix4k, 0x07) & 0xFF000000) >> 8 |
                               ((elapsed / 1000 % 10) << 12) + ((elapsed / 100 % 10) << 8) |
                               ((elapsed / 10 % 10) << 4) + (elapsed % 10);
                calculateBlockChecksum(&currentBlock, i);
                break;
            }

            case 0x11:
            case 0x15:
            case 0x40:
            case 0x44:
                /* Key ID [last three bytes] */
                currentBlock = *SrixGetBlock(key->srix4k, 0x07);
                calculateBlockChecksum(&currentBlock, i);
                break;

            case 0x22:
            case 0x26:
            case 0x51:
            case 0x55: {
                /* Production date (last three bytes) */
                uint32_t *productionDate = SrixGetBlock(key->srix4k, 0x08);
                currentBlock = (*productionDate & 0x0000FF00) << 8 | (*productionDate & 0x00FF0000) >> 8 |
                               (*productionDate & 0xFF000000) >> 24;
                calculateBlockChecksum(&currentBlock, i);
                encodeDecodeBlock(&currentBlock);
                break;
            }

            case 0x12:
            case 0x16:
            case 0x41:
            case 0x45:
                /* Operations counter */
                currentBlock = 1;
                calculateBlockChecksum(&currentBlock, i);
                break;

            case 0x13:
            case 0x17:
            case 0x42:
            case 0x46:
                /* Generic blocks */
                currentBlock = 0x00040013;
                calculateBlockChecksum(&currentBlock, i);
                break;

            case 0x18:
            case 0x1C:
            case 0x47:
            case 0x4B:
                /* Generic blocks */
                currentBlock = 0x0000FEDC;
                calculateBlockChecksum(&currentBlock, i);
                encodeDecodeBlock(&currentBlock);
                break;

            case 0x19:
            case 0x1D:
            case 0x48:
            case 0x4C:
                /* Generic blocks */
                currentBlock = 0x00000123;
                calculateBlockChecksum(&currentBlock, i);
                encodeDecodeBlock(&currentBlock);
                break;

            case 0x21:
            case 0x25:
                /* Current credit (0,00€) */
                calculateEncryptionKey(key);
                currentBlock = 0;
                calculateBlockChecksum(&currentBlock, i);
                encodeDecodeBlock(&currentBlock);
                currentBlock ^= key->encryptionKey;
                break;

            case 0x20:
            case 0x24:
            case 0x4F:
            case 0x53:
                /* Generic blocks */
                currentBlock = 0x00010000;
                calculateBlockChecksum(&currentBlock, i);
                encodeDecodeBlock(&currentBlock);
                break;

            case 0x1A:
            case 0x1B:
            case 0x1E:
            case 0x1F:
            case 0x23:
            case 0x27:
            case 0x49:
            case 0x4A:
            case 0x4D:
            case 0x4E:
            case 0x50:
            case 0x52:
            case 0x54:
            case 0x56:
                /* Generic blocks */
                currentBlock = 0;
                calculateBlockChecksum(&currentBlock, i);
                encodeDecodeBlock(&currentBlock);
                break;

            default:
                currentBlock = 0xFFFFFFFF;
                break;
        }

        /* If this block has a different value than EEPROM, modify it. */
        if (memcmp(SrixGetBlock(key->srix4k, i), &currentBlock, sizeof(uint32_t)) != 0) {
            SrixModifyBlock(key->srix4k, currentBlock, i);
        }
    }
}

uint16_t MyKeyGetCurrentCredit(MyKey key[static 1]) {
    uint32_t currentCredit = *SrixGetBlock(key->srix4k, 0x21) ^ key->encryptionKey;
    encodeDecodeBlock(&currentCredit);
    return currentCredit;
}

int MyKeyAddCents(MyKey key[static 1], uint16_t cents, uint8_t day, uint8_t month, uint8_t year) {
    /* Check lock id */
    if (MyKeyCheckLockID(key)) {
        key->error = MIKAI_ERROR(MIKAI_MYKEY_ERROR,"your key has an unknown protection (lock id) and I can't charge it");
        return MIKAI_MYKEY_ERROR;
    }

    /* Check reset key */
    if (MyKeyIsReset(key)) {
        key->error = MIKAI_ERROR(MIKAI_MYKEY_ERROR, "your mykey isn't associated with any vendor");
        return MIKAI_MYKEY_ERROR;
    }

    if (*SrixGetBlock(key->srix4k, 0x06) == 0) {
        key->error = MIKAI_ERROR(MIKAI_MYKEY_ERROR, "your mykey isn't associated with any vendor");
        return MIKAI_MYKEY_ERROR;
    }

    /* Calculate current credit */
    uint16_t precedentCredit;
    uint16_t actualCredit = MyKeyGetCurrentCredit(key);

    /* Get current transaction position */
    uint8_t current = getCurrentTransactionOffset(key);

    /* Split credit into multiple transaction. Stop at 5 cent. */
    do {
        /* Save current credit to precedent */
        precedentCredit = actualCredit;

        /* Choose current recharge */
        if (cents / 200 > 0) {
            /* 2€ */
            cents -= 200;
            actualCredit += 200;
        } else if (cents / 100 > 0) {
            /* 1€ */
            cents -= 100;
            actualCredit += 100;
        } else if (cents / 50 > 0) {
            /* 0,50€ */
            cents -= 50;
            actualCredit += 50;
        } else if (cents / 20 > 0) {
            /* 0,20€ */
            cents -= 20;
            actualCredit += 20;
        } else if (cents / 10 > 0) {
            /* 0,10€ */
            cents -= 10;
            actualCredit += 10;
        } else if (cents / 5 > 0) {
            /* 0,05€ */
            cents -= 5;
            actualCredit += 5;
        } else {
            /* < 0.05€ */
            cents -= cents;
            actualCredit += cents;
        }

        /* Point to new credit position */
        current = (current == 7) ? 0 : current + 1;

        /* Save new credit to history blocks */
        SrixModifyBlock(key->srix4k, day << 27 | month << 23 | year << 16 | actualCredit, 0x34 + current);
    } while (cents > 5);

    /* Save new credit to 21 and 25 */
    SrixModifyBlock(key->srix4k, actualCredit, 0x21);
    uint32_t *block21 = SrixGetBlock(key->srix4k, 0x21);
    calculateBlockChecksum(block21, 0x21);
    encodeDecodeBlock(block21);
    *block21 ^= key->encryptionKey;

    SrixModifyBlock(key->srix4k, actualCredit, 0x25);
    uint32_t *block25 = SrixGetBlock(key->srix4k, 0x25);
    calculateBlockChecksum(block25, 0x25);
    encodeDecodeBlock(block25);
    *block25 ^= key->encryptionKey;

    /* Save precedent credit to 23 and 27 */
    SrixModifyBlock(key->srix4k, precedentCredit, 0x23);
    uint32_t *block23 = SrixGetBlock(key->srix4k, 0x23);
    calculateBlockChecksum(block23, 0x23);
    encodeDecodeBlock(block23);

    SrixModifyBlock(key->srix4k, precedentCredit, 0x27);
    uint32_t *block27 = SrixGetBlock(key->srix4k, 0x27);
    calculateBlockChecksum(block27, 0x27);
    encodeDecodeBlock(block27);

    /* Save transaction pointer to block 3C */
    SrixModifyBlock(key->srix4k, current << 16, 0x3C);
    uint32_t *block3C = SrixGetBlock(key->srix4k, 0x3C);
    calculateBlockChecksum(block3C, 0x3C);
    encodeDecodeBlock(block3C);
    *block3C ^= *SrixGetBlock(key->srix4k, 0x07) & 0x00FFFFFF;

    return MIKAI_SUCCESS;
}

int MyKeySetCents(MyKey key[static 1], uint16_t cents, uint8_t day, uint8_t month, uint8_t year) {
    /* Dump precedent blocks (restore in case of failure) */
    uint32_t dump[10];
    memcpy(dump, SrixGetBlock(key->srix4k, 0x21), SRIX_BLOCK_LENGTH);
    memcpy(dump + 1, SrixGetBlock(key->srix4k, 0x34), 9 * SRIX_BLOCK_LENGTH);

    uint32_t *block21 = SrixGetBlock(key->srix4k, 0x21);
    *block21 = 0;
    calculateBlockChecksum(block21, 0x21);
    encodeDecodeBlock(block21);
    *block21 ^= key->encryptionKey;

    /* Reset transaction history and pointer (0x24-0x3C) */
    memset(SrixGetBlock(key->srix4k, 0x34), 0xFF, 9 * SRIX_BLOCK_LENGTH);

    /* If there is an error, restore precedent dump */
    if (MyKeyAddCents(key, cents, day, month, year) != MIKAI_SUCCESS) {
        memcpy(SrixGetBlock(key->srix4k, 0x21), dump, SRIX_BLOCK_LENGTH);
        memcpy(SrixGetBlock(key->srix4k, 0x34), dump + 1, 9 * SRIX_BLOCK_LENGTH);
        return MIKAI_MYKEY_ERROR;
    } else {
        return MIKAI_SUCCESS;
    }
}
