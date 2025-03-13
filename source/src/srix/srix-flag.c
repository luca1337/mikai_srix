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

#include "srix-flag.h"

void srixFlagAdd(SrixFlag flag[static 1], uint8_t block) {
    /*
     * Array:
     * 0 -> 0-31
     * 1-> 32-63
     * 2-> 64-95
     * 3-> 96-127
     *
     * Flag position bit between 0 and 31 in a single uint32_t
     * (array of 4 uint32_t).
     */
    if (block < 128) {
        flag->memory[block / 32] |= 1U << block % 32;
    }
}

bool srixFlagGet(SrixFlag flag[static 1], uint8_t block) {
    if (block < 128) {
        return flag->memory[block / 32] >> block % 32 & 1U;
    } else {
        return false;
    }
}