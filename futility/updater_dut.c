/* Copyright 2023 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * The DUT interface helper functions for the firmware updater.
 */

#include <assert.h>
#ifdef HAVE_CROSID
#include <crosid.h>
#endif
#include "crossystem.h"
#include "updater.h"

/**
 * dut_get_manifest_key() - Wrapper to get the firmware manifest key from crosid
 *
 * @manifest_key_out - Output parameter of the firmware manifest key.
 *
 * Returns:
 * - <0 if libcrosid is unavailable or there was an error reading
 *   device data
 * - >=0 (the matched device index) success
 */
int dut_get_manifest_key(char **manifest_key_out)
{
#ifdef HAVE_CROSID
	return crosid_get_firmware_manifest_key(manifest_key_out);
#else
	ERROR("This version of futility was compiled without libcrosid "
	      "(perhaps compiled outside of the Chrome OS build system?) and "
	      "the update command is not fully supported.  Either compile "
	      "from the Chrome OS build, or pass --model to manually specify "
	      "the machine model.\n");
	return -1;
#endif
}

int dut_set_property_string(const char *key, const char *value)
{
	return VbSetSystemPropertyString(key, value);
}

const char *dut_get_property_string(const char *key, char *dest, size_t size)
{
	return VbGetSystemPropertyString(key, dest, size);
}

int dut_set_property_int(const char *key, const int value)
{
	return VbSetSystemPropertyInt(key, value);
}

int dut_get_property_int(const char *key)
{
	return VbGetSystemPropertyInt(key);
}

/* An helper function to return "mainfw_act" system property.  */
static int dut_get_mainfw_act(struct updater_config *cfg)
{
	char buf[VB_MAX_STRING_PROPERTY];

	if (!dut_get_property_string("mainfw_act", buf, sizeof(buf)))
		return SLOT_UNKNOWN;

	if (strcmp(buf, FWACT_A) == 0)
		return SLOT_A;
	else if (strcmp(buf, FWACT_B) == 0)
		return SLOT_B;

	return SLOT_UNKNOWN;
}

/* A helper function to return the "tpm_fwver" system property. */
static int dut_get_tpm_fwver(struct updater_config *cfg)
{
	return dut_get_property_int("tpm_fwver");
}

/* A helper function to return the "hardware write protection" status. */
static int dut_get_wp_hw(struct updater_config *cfg)
{
	/* wpsw refers to write protection 'switch', not 'software'. */
	return dut_get_property_int("wpsw_cur") ? WP_ENABLED : WP_DISABLED;
}

/* A helper function to return "fw_vboot2" system property. */
static int dut_get_fw_vboot2(struct updater_config *cfg)
{
	return dut_get_property_int("fw_vboot2");
}

static int dut_get_platform_version(struct updater_config *cfg)
{
	return dut_get_property_int("board_id");
}

/* Helper function to return host software write protection status. */
static int dut_get_wp_sw(struct updater_config *cfg)
{
	return flashrom_get_wp(PROG_HOST, -1);
}

/* Helper functions to use or configure the DUT properties. */

/*
 * Gets the DUT system property by given type.
 * If the property was not loaded yet, invoke the property getter function
 * and cache the result.
 * Returns the property value.
 */
int dut_get_property(enum dut_property_type property_type,
		     struct updater_config *cfg)
{
	struct dut_property *prop;

	assert(property_type < DUT_PROP_MAX);
	prop = &cfg->dut_properties[property_type];
	if (!prop->initialized) {
		prop->initialized = 1;
		prop->value = prop->getter(cfg);
	}
	return prop->value;
}

void dut_init_properties(struct dut_property *props, int num)
{
	memset(props, 0, num * sizeof(*props));
	assert(num >= DUT_PROP_MAX);
	props[DUT_PROP_MAINFW_ACT].getter = dut_get_mainfw_act;
	props[DUT_PROP_TPM_FWVER].getter = dut_get_tpm_fwver;
	props[DUT_PROP_FW_VBOOT2].getter = dut_get_fw_vboot2;
	props[DUT_PROP_PLATFORM_VER].getter = dut_get_platform_version;
	props[DUT_PROP_WP_HW].getter = dut_get_wp_hw;
	props[DUT_PROP_WP_SW].getter = dut_get_wp_sw;
}