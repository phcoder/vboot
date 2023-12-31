/* Copyright 2010 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef VBOOT_REFERENCE_CRC32_H_
#define VBOOT_REFERENCE_CRC32_H_

#include "2sysincludes.h"

uint32_t Crc32(const void *buffer, uint32_t len);

#endif  /* VBOOT_REFERENCE_CRC32_H_ */
