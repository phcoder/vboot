/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#ifndef __SRC_LIB_TPM2_MARSHALING_H
#define __SRC_LIB_TPM2_MARSHALING_H

#include "tss_constants.h"

/* The below functions are used to serialize/deserialize TPM2 commands. */

/**
 * tpm_marshal_command
 *
 * Given a structure containing a TPM2 command, serialize the structure for
 * sending it to the TPM.
 *
 * @command: code of the TPM2 command to marshal
 * @tpm_command_body: a pointer to the command specific structure
 * @buffer: buffer where command is marshaled to
 * @buffer_size: size of the buffer
 *
 * Returns number of bytes placed in the buffer, or -1 on error.
 *
 */
int tpm_marshal_command(TPM_CC command, void *tpm_command_body,
			void *buffer, int buffer_size);

/**
 * tpm_unmarshal_response
 *
 * Given a buffer received from the TPM in response to a certain command,
 * deserialize the buffer into the expeced response structure.
 *
 * struct tpm2_response is a union of all possible responses.
 *
 * @command: code of the TPM2 command for which a response is unmarshaled
 * @response_body: buffer containing the serialized response.
 * @response_size: number of bytes in the buffer containing response
 *
 * Returns a pointer to the deserialized response or NULL in case of
 * unmarshaling problems.
 */
struct tpm2_response *tpm_unmarshal_response(TPM_CC command,
					     void *response_body,
					     int response_size);

#endif // __SRC_LIB_TPM2_MARSHALING_H