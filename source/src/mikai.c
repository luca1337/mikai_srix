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

#include <mikai/mikai.h>
#include "srix/srix.h"
#include "mikai-internal.h"
#include "mikai-error.h"

const char *MikaiVersion() {
    return "4.1.0";
}

MyKey *MikaiNew() {
    /* Allocate mikai struct and check if there are errors */
    MyKey *target = malloc(sizeof(MyKey));
    if (!target) return (void *) 0;

    /* Allocate Srix struct and check if there are errors */
    target->srix4k = SrixNew();
    if (!target->srix4k) {
        MikaiDelete(target);
        return (void *) 0;
    }

    /* Reset MyKey ID */
    *SrixGetBlock(target->srix4k, 0x07) = 0;

    /* Reset error */
    target->error = MIKAI_NO_ERROR;
    target->error.message = "";

    return target;
}

const char *MikaiInit(MyKey **key, uint32_t dump[const SRIX4K_BLOCKS], uint64_t selection) {
    if (!key) {
        return "pointer is null";
    }

    if (!*key) {
        /* Pointer to mikai pointer is null */
        *key = MikaiNew();
        if (!*key) {
            return "unable to allocate memory for MyKey";
        }
    }

    if (dump) {
        /* Initialize Srix from memory */
        SrixMemoryInit((*key)->srix4k, dump, selection);
    } else {
        /* Initialize Srix from NFC */
        (*key)->error = SrixNfcInit((*key)->srix4k, selection);
        if ((*key)->error.errorType != MIKAI_SUCCESS) {
            const char *error = (*key)->error.message;
            MikaiDelete(*key);
            return error;
        }
    }

    /* Calculate MyKey keys */
    void calculateEncryptionKey(MyKey *key);
    calculateEncryptionKey(*key);

    /* Return pointer */
    return (void *) 0;
}

void MikaiDelete(MyKey *key) {
    SrixDelete(key->srix4k);
    free(key);
}

const char *MikaiGetLatestError(MyKey key[static 1]) {
    const char *message = key->error.message;

    /* Reset error */
    key->error = MIKAI_NO_ERROR;
    key->error.message = "";

    return message;
}

size_t MyKeyGetReadersCount(MyKey key[static 1]) {
    return NfcGetReadersCount(key->srix4k);
}

char *MyKeyGetReaderDescription(MyKey key[static 1], int reader) {
    return NfcGetDescription(key->srix4k, reader);
}

int MyKeyWriteAll(MyKey key[static 1]) {
    key->error = SrixWriteBlocks(key->srix4k);
    return key->error.errorType;
}
