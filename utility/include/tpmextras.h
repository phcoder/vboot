/* Copyright 2010 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * TPM definitions not available in any TSS include file :-(
 */

#ifndef VBOOT_REFERENCE_TPMEXTRAS_H_
#define VBOOT_REFERENCE_TPMEXTRAS_H_

#define TPM_MAX_COMMAND_SIZE 4096
#define TPM_LARGE_ENOUGH_COMMAND_SIZE 256  /* saves space in the firmware */
#define TPM_ENCAUTH_SIZE 20
#define TPM_PUBEK_SIZE 256

typedef struct tdTPM_WRITE_INFO {
  uint32_t nvIndex;
  uint32_t offset;
  uint32_t dataSize;
} TPM_WRITE_INFO;

#endif  /* VBOOT_REFERENCE_TPMEXTRAS_H_ */
