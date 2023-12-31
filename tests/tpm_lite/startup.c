/* Copyright 2010 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Only perform a TPM_Startup command.
 */

#include <stdio.h>

#include "tlcl.h"

int main(int argc, char** argv) {
	uint32_t result;
	TlclLibInit();
	result = TlclStartup();
	if (result != 0) {
		printf("tpm startup failed with %#x\n", result);
	}
	result = TlclGetFlags(NULL, NULL, NULL);
	if (result != 0) {
		printf("tpm getflags failed with %#x\n", result);
	}
	printf("executing SelfTestFull\n");
	TlclSelfTestFull();
	result = TlclGetFlags(NULL, NULL, NULL);
	if (result != 0) {
		printf("tpm getflags failed with %#x\n", result);
	}
	printf("TEST SUCCEEDED\n");
	return 0;
}
