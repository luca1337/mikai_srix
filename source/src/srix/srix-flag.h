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

#ifndef MIKAI_SRIX_FLAG_H
#define MIKAI_SRIX_FLAG_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Struct that represents the modified blocks in a SRIX tag
 */
typedef struct SrixFlag {
    uint32_t memory[4];
} SrixFlag;

#define SRIX_FLAG_INIT (SrixFlag) {{0, 0, 0, 0}}

/**
 * Set the flag value of a specified block to true (modified).
 * @param flag pointer to a SrixFlag instance
 * @param block block to flag (0-127)
 */
void srixFlagAdd(SrixFlag *flag, uint8_t block);

/**
 * Get the flag value of a specified block.
 * @param flag pointer to a SrixFlag instance
 * @param block block to get (0-127)
 * @return boolean result
 */
bool srixFlagGet(SrixFlag *flag, uint8_t block);

#endif /* MIKAI_SRIX_FLAG_H */