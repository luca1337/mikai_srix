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

#ifndef MIKAI_MIKAI_INTERNAL_H
#define MIKAI_MIKAI_INTERNAL_H

#include <stdint.h>
#include "srix/srix.h"
#include "mikai-error.h"

/**
 * Struct that represents a MyKey
 */
struct MyKey {
    uint32_t encryptionKey;  /* Session Key */
    Srix *srix4k;            /* SRIX4k tag */
    MikaiError error;        /* Error */
};

#endif /* MIKAI_MIKAI_INTERNAL_H */
