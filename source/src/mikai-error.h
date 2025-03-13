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

#ifndef MIKAI_UTILS_H
#define MIKAI_UTILS_H

#include <stdint.h>

/**
 * Error codes enum.
 * SUCCESS = 0.
 */
typedef enum {
    MIKAI_SUCCESS,
    MIKAI_NFC_ERROR = INT8_MIN,
    MIKAI_SRIX_ERROR,
    MIKAI_MYKEY_ERROR
} MikaiErrorCode;

/**
 * Error structure that contains a
 * description message.
 */
typedef struct MikaiError {
    MikaiErrorCode errorType;
    char const *message;
} MikaiError;

#define MIKAI_NO_ERROR                     (MikaiError) {.errorType = MIKAI_SUCCESS}
#define MIKAI_ERROR(type, errorMessage)    (MikaiError) {.errorType = (type), .message = (errorMessage)}
#define MIKAI_IS_ERROR(isError)            ((isError).errorType != MIKAI_SUCCESS)


#endif /* MIKAI_LOG_H */