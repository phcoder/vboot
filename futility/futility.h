/* Copyright 2013 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef VBOOT_REFERENCE_FUTILITY_H_
#define VBOOT_REFERENCE_FUTILITY_H_

#include <stdint.h>

#include "2common.h"
#include "host_key.h"

/* This program */
#define MYNAME "futility"

/* Version string (autogenerated) */
extern const char futility_version[];

/* Bitfields indicating the struct/format versions supported by a command */
enum vboot_version  {
	/*
	 * v1.0 is the original structs used since the dawn of time.
	 * v2.0 can verify the firmware in smaller chunks, but there's
	 * no difference in the on-device structs, so it's only
	 * meaningful for the firmware API. Futility doesn't care.
	 */
	VBOOT_VERSION_1_0 = 0x00000001,

	/*
	 * v2.1 uses new and different structs, and is what v2.0 would have
	 * been if someone hadn't started using it before it was ready.
	 */
	VBOOT_VERSION_2_1 = 0x00000002,

	/*
	 * Everything we know about to date.
	 */
	VBOOT_VERSION_ALL = 0x00000003,
};

/* What's our preferred API & data format? */
extern enum vboot_version vboot_version;

/* Here's a structure to define the commands that futility implements. */
struct futil_cmd_t {
	/* String used to invoke this command */
	const char *const name;
	/* Function to do the work. Returns 0 on success.
	 * Called with argv[0] == "name".
	 * It should handle its own "--help" option. */
	int (*const handler) (int argc, char **argv);
	/* Supported ABIs */
	enum vboot_version version;
	/* One-line summary of what it does */
	const char *const shorthelp;
};

/* Macro to define a command */
#define DECLARE_FUTIL_COMMAND(NAME, HANDLER, VERSION, SHORTHELP)	\
	const struct futil_cmd_t __cmd_##NAME = {			\
		.name = #NAME,						\
		.handler = HANDLER,					\
		.version = VERSION,					\
		.shorthelp = SHORTHELP,					\
	}

/* This is the list of pointers to all commands. */
extern const struct futil_cmd_t *const futil_cmds[];

/* Size of an array */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#endif

/* Fatal error (print error message and exit). */
#define FATAL(format, ...) do { \
		fprintf(stderr, "FATAL: %s: " format, __func__, \
			##__VA_ARGS__ ); \
		exit(1); \
	} while (0)

/* Print error messages (won't exit). */
#define ERROR(format, ...) fprintf(stderr, "ERROR: %s: " format, __func__, \
				   ##__VA_ARGS__ )
#define WARN(format, ...) fprintf(stderr, "WARNING: %s: " format, __func__, \
				  ##__VA_ARGS__ )
#define INFO(format, ...) fprintf(stderr, "INFO: %s: " format, __func__, \
				  ##__VA_ARGS__ )
#define STATUS(format, ...) fprintf(stderr, ">> " format, ##__VA_ARGS__ )

extern const char *ft_print_header;
extern const char *ft_print_header2;

/*
 * `futility show` supports two kinds of output: human readable and machine
 * parseable. To keep the code simple, there is mostly a 1-to-1 relationship
 * between lines of human readable and lines of machine parseable output. The
 * FT_PRINT() macro should be used in those cases to define both output types
 * for a given line in one statement. In rare cases where lines do not match
 * 1-to-1, FT_READABLE_PRINT() and FT_PARSEABLE_PRINT() can be used to only
 * print to one or the other format.
 *
 * The requirements for machine parseable output are as follows and must be
 * strictly followed to ensure backwards-compatibility with tools using it:
 *
 * * Each line consists of a string of prefix tokens and one or more data
 *   values, separated by double colon delimiters (`::`).
 *
 *   * Output lines are independent of each other. No information may be encoded
 *     by the position of one output line relative to another (e.g. no
 *     "headings" which apply to all following lines).
 *
 *   * Tokens should form a hierarchy that groups related values together.
 *
 *     * Do not just use delimiters to separate words. Each token should
 *       represent a real layer in the hierarchy (e.g.
 *       `preamble::firmware_version` instead of `preamble::firmware::version`
 *       since "firmware" isn't a real hierarchical layer within the preamble).
 *
 *     * The toplevel token should be the same for all output lines and
 *       represent the futility file type being displayed (e.g. `bios`).
 *
 *   * Neither tokens nor values may contain the colon character (`:`) or a
 *     line break (`\n`).
 *
 *   * All integer values (even memory addresses) should be output in decimal.
 *
 *   * Every line should represent one single piece of information (i.e. no
 *     aggregate lines that e.g. show both the offset and size of something in
 *     the same line).
 *
 *     * If a single piece of information can be represented in multiple ways
 *       (e.g. hash algorithm by name and numerical ID), and it is useful to
 *       output both of them, they should be both output on the same line
 *       separated by `::`.
 *
 *   * Values should generally be simple. For very complex values and those that
 *     need to contain the disallowed characters or raw binary data (like the
 *     kernel command line), consider just creating a separate command to
 *     extract them specifically (e.g. `futility dump_kernel_config`).
 *
 *   * When a line represents a set (e.g. flags in textual representation), the
 *     individual set items should be separated by single colons (`:`). Callers
 *     should make no assumptions about the order of items listed in a set.
 *
 * * The parseable output is considered a stable API. Once an output line has
 *   been added, futility must forever return the exact same format (same prefix
 *   tokens, same data values in the same notation) on the same input file.
 *
 *   * Output lines are independent. Callers must make no assumption about the
 *     order of output lines, and additional lines may be added in the future.
 *
 *   * For values representing a set, new items may become possible for the set
 *     in future versions as long as the existing ones are still represented in
 *     the same way.
 *
 *   * When input files themselves change in a way that certain output lines no
 *     longer make sense for them (e.g. switching from raw area signing to
 *     metadata hash signing), some of the output lines that used to appear for
 *     the old version of that file type may no longer appear for the new
 *     version. But the lines that do appear must still follow the same
 *     format as they did for the old version.
 *
 *   * When there's a strong need to change the existing way something is
 *     represented, a new output line should be added that represents it in a
 *     better way. The old output line should be marked deprecated in a code
 *     comment but not removed or altered in any way from the output. (This
 *     means that as changes accrue information may be displayed in multiple
 *     redundant ways.)
 *
 *   * If one day the burden of accumulated deprecated output lines becomes too
 *     high, we may consider a permanent deprecation and removal plan. But this
 *     would be done over a long time frame with plenty of heads-up notice to
 *     `futility show` consumers to ensure they have migrated to the new format.
 *     Consumers are meant to be able to trust that they can hardcode parsing
 *     for a certain output line and it will remain working without futility
 *     suddenly pulling the rug out from under them.
 */

/* futility print helpers to handle parseable prints */
#define FT_READABLE_PRINT(fmt, args...) do { \
		if (!show_option.parseable) \
			printf(fmt, ##args); \
	} while (0)

#define FT_PARSEABLE_PRINT(fmt, args...) do { \
		if (!show_option.parseable) \
			break; \
		if (ft_print_header != NULL) \
			printf("%s::", ft_print_header); \
		if (ft_print_header2 != NULL) \
			printf("%s::", ft_print_header2); \
		printf(fmt, ##args); \
	} while (0)

#define FT_PRINT_RAW(normal_fmt, parse_fmt, args...) \
	printf(show_option.parseable ? parse_fmt : normal_fmt, ##args)

#define FT_PRINT(normal_fmt, parse_fmt, args...) do { \
		FT_READABLE_PRINT(normal_fmt, ##args); \
		FT_PARSEABLE_PRINT(parse_fmt, ##args); \
	} while (0)

/* Debug output (off by default) */
extern int debugging_enabled;

/* Returns true if this looks enough like a GBB header to proceed. */
int futil_looks_like_gbb(struct vb2_gbb_header *gbb, uint32_t len);

/*
 * Returns true if the gbb header is valid (and optionally updates *maxlen).
 * This doesn't verify the contents, though.
 */
int futil_valid_gbb_header(struct vb2_gbb_header *gbb, uint32_t len,
			   uint32_t *maxlen);

/* Sets the HWID string field inside a GBB header. */
int futil_set_gbb_hwid(struct vb2_gbb_header *gbb, const char *hwid);

/* For GBB v1.2 and later, update the hwid_digest */
void update_hwid_digest(struct vb2_gbb_header *gbb);

/* For GBB v1.2 and later, print the stored digest of the HWID (and whether
 * it's correct). Return true if it is correct. */
int print_hwid_digest(struct vb2_gbb_header *gbb, const char *banner);

/* Copies a file. */
int futil_copy_file(const char *infile, const char *outfile);

/* Possible file operation errors */
enum futil_file_err {
	FILE_ERR_NONE,
	FILE_ERR_STAT,
	FILE_ERR_SIZE,
	FILE_ERR_MMAP,
	FILE_ERR_MSYNC,
	FILE_ERR_MUNMAP,
	FILE_ERR_OPEN,
	FILE_ERR_CLOSE,
	FILE_ERR_DIR,
	FILE_ERR_CHR,
	FILE_ERR_FIFO,
	FILE_ERR_SOCK,
};

enum file_mode {
	FILE_RO,
	FILE_RW,
};

enum futil_file_err futil_open_file(const char *infile, int *fd,
				    enum file_mode mode);
enum futil_file_err futil_close_file(int fd);

/* Wrapper for mmap/munmap. Skips stupidly large files. */
enum futil_file_err futil_map_file(int fd, enum file_mode mode, uint8_t **buf,
				   uint32_t *len);
enum futil_file_err futil_unmap_file(int fd, enum file_mode mode, uint8_t *buf,
				     uint32_t len);

enum futil_file_err futil_open_and_map_file(const char *infile, int *fd,
					    enum file_mode mode, uint8_t **buf,
					    uint32_t *len);
enum futil_file_err futil_unmap_and_close_file(int fd, enum file_mode mode,
					       uint8_t *buf, uint32_t len);

/*
 * Parse input string as a hex representation of size len, exit with error if
 *  the string is not a valid hex string or is of a wrongs size.
 */
void parse_digest_or_die(uint8_t *buf, int len, const char *str);

/*
 * Print provided buffer as hex string
 */
void print_bytes(const void *ptr, size_t len);

/* The CPU architecture is occasionally important */
enum arch_t {
	ARCH_UNSPECIFIED,
	ARCH_X86,
	ARCH_ARM,
	ARCH_MIPS
};

/*
 * Write size bytes from start into filename. Print "%msg %filename" to stdout
 * on success, if msg is non-NULL. Writes messages to stderr on failure.
 * Returns 0 on success.
 */
int write_to_file(const char *msg, const char *filename, uint8_t *start,
		  size_t size);

#endif  /* VBOOT_REFERENCE_FUTILITY_H_ */
