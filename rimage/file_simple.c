/*
 * ELF to firmware image creator.
 *
 * Copyright (c) 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "rimage.h"
#include "file_format.h"

#define BYT_IRAM_BASE		0xff2c0000
#define BYT_IRAM_HOST_OFFSET	0x0C0000
#define BYT_IRAM_SIZE		(80 * 1024)
#define BYT_DRAM_BASE		0xff300000
#define BYT_DRAM_HOST_OFFSET	0x100000
#define BYT_DRAM_SIZE		(160 * 1024)

#define HSW_IRAM_BASE		0x00000000
#define HSW_IRAM_HOST_OFFSET	0x00080000
#define HSW_IRAM_SIZE		(384 * 1024)
#define HSW_DRAM_BASE		0x00400000
#define HSW_DRAM_HOST_OFFSET	0x00000000
#define HSW_DRAM_SIZE		(512 * 1024)

#define BDW_IRAM_BASE		0x00000000
#define BDW_IRAM_HOST_OFFSET	0x000A0000
#define BDW_IRAM_SIZE		(320 * 1024)
#define BDW_DRAM_BASE		0x00400000
#define BDW_DRAM_HOST_OFFSET	0x00000000
#define BDW_DRAM_SIZE		(640 * 1024)

#define HKY_IRAM_BASE		0xE8080000
#define HKY_IRAM_HOST_OFFSET	0x00000000
#define HKY_IRAM_SIZE		(48 * 1024)
#define HKY_DRAM_BASE		0xE8058000
#define HKY_DRAM_HOST_OFFSET	0x00000000
#define HKY_DRAM_SIZE		(160 * 1024)

static int get_mem_zone_type(struct image *image, Elf32_Shdr *section)
{
	const struct adsp *adsp = image->adsp;
	uint32_t start, end, base, size;
	int i;

	start = section->vaddr;
	end = section->vaddr + section->size;

	for (i = SOF_FW_BLK_TYPE_START; i < SOF_FW_BLK_TYPE_NUM; i++) {
		base =  adsp->mem_zones[i].base;
		size =  adsp->mem_zones[i].size;

		if (start < base)
			continue;
		if (start >= base + size)
			continue;
		if (end > base + size)
			continue;
		return i;
	}
	return SOF_FW_BLK_TYPE_INVALID;
}

static int block_idx;

static int write_block(struct image *image, struct module *module,
		       Elf32_Shdr *section)
{
	const struct adsp *adsp = image->adsp;
	struct snd_sof_blk_hdr block;
	uint32_t padding = 0;
	size_t count;
	void *buffer;
	int ret;

	block.size = section->size;
	if (block.size % 4) {
		/* make block.size divisible by 4 to avoid unaligned accesses */
		padding = 4 - (block.size % 4);
		block.size += padding;
	}

	ret = get_mem_zone_type(image, section);
	if (ret != SOF_FW_BLK_TYPE_INVALID) {
		block.type = ret;
		block.offset = section->vaddr - adsp->mem_zones[ret].base
			+ adsp->mem_zones[ret].host_offset;
	} else {
		fprintf(stderr, "error: invalid block address/size 0x%x/0x%x\n",
			section->vaddr, section->size);
		return -EINVAL;
	}

	/* write header */
	count = fwrite(&block, sizeof(block), 1, image->out_fd);
	if (count != 1)
		return -errno;

	/* alloc data data */
	buffer = calloc(1, section->size);
	if (!buffer)
		return -ENOMEM;

	/* read in section data */
	ret = fseek(module->fd, section->off, SEEK_SET);
	if (ret < 0) {
		fprintf(stderr, "error: cant seek to section %d\n", ret);
		goto out;
	}
	count = fread(buffer, 1, section->size, module->fd);
	if (count != section->size) {
		fprintf(stderr, "error: cant read section %d\n", -errno);
		ret = -errno;
		goto out;
	}

	/* write out section data */
	count = fwrite(buffer, 1, block.size, image->out_fd);
	if (count != block.size) {
		fprintf(stderr, "error: cant write section %d\n", -errno);
		fprintf(stderr, " foffset %d size 0x%x mem addr 0x%x\n",
			section->off, section->size, section->vaddr);
		ret = -errno;
		goto out;
	}

	fprintf(stdout, "\t%d\t0x%8.8x\t0x%8.8x\t0x%8.8lx\t%s\n", block_idx++,
		section->vaddr, section->size, ftell(image->out_fd),
		block.type == SOF_FW_BLK_TYPE_IRAM ? "TEXT" : "DATA");

out:
	free(buffer);
	/* return padding size */
	if (ret >= 0)
		return padding;

	return ret;
}

static int simple_write_module(struct image *image, struct module *module)
{
	struct snd_sof_mod_hdr hdr;
	Elf32_Shdr *section;
	size_t count;
	int i, err;
	uint32_t valid = (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR);
	int ptr_hdr, ptr_cur;
	uint32_t padding = 0;

	hdr.num_blocks = module->num_sections - module->num_bss;
	hdr.size = module->text_size + module->data_size +
		sizeof(struct snd_sof_blk_hdr) * hdr.num_blocks;
	hdr.type = SOF_FW_BASE;

	/* Get the pointer of writing hdr */
	ptr_hdr = ftell(image->out_fd);
	count = fwrite(&hdr, sizeof(hdr), 1, image->out_fd);
	if (count != 1) {
		fprintf(stderr, "error: failed to write section header %d\n",
			-errno);
		return -errno;
	}

	fprintf(stdout, "\n\tTotals\tStart\t\tEnd\t\tSize");

	fprintf(stdout, "\n\tTEXT\t0x%8.8x\t0x%8.8x\t0x%x\n",
		module->text_start, module->text_end,
		module->text_end - module->text_start);
	fprintf(stdout, "\tDATA\t0x%8.8x\t0x%8.8x\t0x%x\n",
		module->data_start, module->data_end,
		module->data_end - module->data_start);
	fprintf(stdout, "\tBSS\t0x%8.8x\t0x%8.8x\t0x%x\n\n ",
		module->bss_start, module->bss_end,
		module->bss_end - module->bss_start);

	fprintf(stdout, "\tNo\tAddress\t\tSize\t\tFile\t\tType\n");

	for (i = 0; i < module->hdr.shnum; i++) {
		section = &module->section[i];

		/* only write valid sections */
		if (!(module->section[i].flags & valid))
			continue;

		/* dont write bss */
		if (section->type == SHT_NOBITS)
			continue;

		err = write_block(image, module, section);
		if (err < 0) {
			fprintf(stderr, "error: failed to write section #%d\n",
				i);
			return err;
		}
		/* write_block will return padding size */
		padding += err;
	}
	hdr.size += padding;
	/* Record current pointer, will set it back after overwriting hdr */
	ptr_cur = ftell(image->out_fd);
	/* overwrite hdr */
	fseek(image->out_fd, ptr_hdr, SEEK_SET);
	count = fwrite(&hdr, sizeof(hdr), 1, image->out_fd);
	if (count != 1) {
		fprintf(stderr, "error: failed to write section header %d\n",
			-errno);
		return -errno;
	}
	fseek(image->out_fd, ptr_cur, SEEK_SET);

	fprintf(stdout, "\n");
	/* return padding size */
	return padding;
}

static int write_block_reloc(struct image *image, struct module *module)
{
	struct snd_sof_blk_hdr block;
	size_t count;
	void *buffer;
	int ret;

	block.size = module->file_size;
	block.type = SOF_FW_BLK_TYPE_DRAM;
	block.offset = 0;

	/* write header */
	count = fwrite(&block, sizeof(block), 1, image->out_fd);
	if (count != 1)
		return -errno;

	/* alloc data data */
	buffer = calloc(1, module->file_size);
	if (!buffer)
		return -ENOMEM;

	/* read in section data */
	ret = fseek(module->fd, 0, SEEK_SET);
	if (ret < 0) {
		fprintf(stderr, "error: can't seek to section %d\n", ret);
		goto out;
	}
	count = fread(buffer, 1, module->file_size, module->fd);
	if (count != module->file_size) {
		fprintf(stderr, "error: can't read section %d\n", -errno);
		ret = -errno;
		goto out;
	}

	/* write out section data */
	count = fwrite(buffer, 1, module->file_size, image->out_fd);
	if (count != module->file_size) {
		fprintf(stderr, "error: can't write section %d\n", -errno);
		ret = -errno;
		goto out;
	}

	fprintf(stdout, "\t%d\t0x%8.8x\t0x%8.8x\t0x%8.8lx\t%s\n", block_idx++,
		0, module->file_size, ftell(image->out_fd),
		block.type == SOF_FW_BLK_TYPE_IRAM ? "TEXT" : "DATA");

out:
	free(buffer);
	return ret;
}

static int simple_write_module_reloc(struct image *image, struct module *module)
{
	struct snd_sof_mod_hdr hdr;
	size_t count;
	int err;

	hdr.num_blocks = 1;
	hdr.size = module->text_size + module->data_size;
	hdr.type = SOF_FW_BASE; // module

	count = fwrite(&hdr, sizeof(hdr), 1, image->out_fd);
	if (count != 1) {
		fprintf(stderr, "error: failed to write section header %d\n",
			-errno);
		return -errno;
	}

	fprintf(stdout, "\n\tTotals\tStart\t\tEnd\t\tSize");

	fprintf(stdout, "\n\tTEXT\t0x%8.8x\t0x%8.8x\t0x%x\n",
		module->text_start, module->text_end,
		module->text_end - module->text_start);
	fprintf(stdout, "\tDATA\t0x%8.8x\t0x%8.8x\t0x%x\n",
		module->data_start, module->data_end,
		module->data_end - module->data_start);
	fprintf(stdout, "\tBSS\t0x%8.8x\t0x%8.8x\t0x%x\n\n ",
		module->bss_start, module->bss_end,
		module->bss_end - module->bss_start);

	fprintf(stdout, "\tNo\tAddress\t\tSize\t\tFile\t\tType\n");

	err = write_block_reloc(image, module);
	if (err < 0) {
		fprintf(stderr, "error: failed to write section #%d\n", err);
		return err;
	}

	fprintf(stdout, "\n");
	return 0;
}

/* used by others */
static int simple_write_firmware(struct image *image)
{
	struct snd_sof_fw_header hdr;
	struct module *module;
	size_t count;
	int i, ret;

	memcpy(hdr.sig, SND_SOF_FW_SIG, SND_SOF_FW_SIG_SIZE);

	hdr.num_modules = image->num_modules;
	hdr.abi = SND_SOF_FW_ABI;
	hdr.file_size = 0;

	for (i = 0; i < image->num_modules; i++) {
		module = &image->module[i];
		module->fw_size += sizeof(struct snd_sof_blk_hdr) *
				(module->num_sections - module->num_bss);
		module->fw_size += sizeof(struct snd_sof_mod_hdr) *
				hdr.num_modules;
		hdr.file_size += module->fw_size;
	}

	count = fwrite(&hdr, sizeof(hdr), 1, image->out_fd);
	if (count != 1)
		return -errno;

	for (i = 0; i < image->num_modules; i++) {
		module = &image->module[i];

		fprintf(stdout, "writing module %d %s\n", i, module->elf_file);

		if (image->reloc)
			ret = simple_write_module_reloc(image, module);
		else
			ret = simple_write_module(image, module);
		if (ret < 0) {
			fprintf(stderr, "error: failed to write module %d\n",
				i);
			return ret;
		}
		/* add padding size */
		hdr.file_size += ret;
	}
	/* overwrite hdr */
	fseek(image->out_fd, 0, SEEK_SET);
	count = fwrite(&hdr, sizeof(hdr), 1, image->out_fd);
	if (count != 1)
		return -errno;

	fprintf(stdout, "firmware: image size %ld (0x%lx) bytes %d modules\n\n",
		(long)(hdr.file_size + sizeof(hdr)),
		(long)(hdr.file_size + sizeof(hdr)),
		hdr.num_modules);

	return 0;
}

int write_logs_dictionary(struct image *image)
{
	struct snd_sof_logs_header header;
	int i, ret = 0;
	void *buffer = NULL;

	memcpy(header.sig, SND_SOF_LOGS_SIG, SND_SOF_LOGS_SIG_SIZE);
	header.data_offset = sizeof(struct snd_sof_logs_header);

	for (i = 0; i < image->num_modules; i++) {
		struct module *module = &image->module[i];

		/* extract fw_version from fw_ready message located
		 * in .fw_ready section
		 */
		if (module->fw_ready_index > 0) {
			Elf32_Shdr *section =
				&module->section[module->fw_ready_index];

			buffer = calloc(1, sizeof(struct sof_ipc_fw_ready));
			if (!buffer)
				return -ENOMEM;

			fseek(module->fd, section->off, SEEK_SET);
			size_t count = fread(buffer, 1,
				sizeof(struct sof_ipc_fw_ready), module->fd);

			if (count != sizeof(struct sof_ipc_fw_ready)) {
				fprintf(stderr,
					"error: can't read ready section %d\n",
					-errno);
				ret = -errno;
				goto out;
			}

			memcpy(&header.version,
			       &((struct sof_ipc_fw_ready *)buffer)->version,
			       sizeof(header.version));

			free(buffer);
			buffer = NULL;
		}

		if (module->logs_index > 0) {
			Elf32_Shdr *section =
				&module->section[module->logs_index];

			header.base_address = section->vaddr;
			header.data_length = section->size;

			fwrite(&header, sizeof(struct snd_sof_logs_header), 1,
			       image->ldc_out_fd);

			buffer = calloc(1, section->size);
			if (!buffer)
				return -ENOMEM;

			fseek(module->fd, section->off, SEEK_SET);
			size_t count = fread(buffer, 1, section->size,
				module->fd);
			if (count != section->size) {
				fprintf(stderr,
					"error: can't read logs section %d\n",
					-errno);
				ret = -errno;
				goto out;
			}
			count = fwrite(buffer, 1, section->size,
				       image->ldc_out_fd);
			if (count != section->size) {
				fprintf(stderr,
					"error: can't write section %d\n",
					-errno);
				ret = -errno;
				goto out;
			}

			fprintf(stdout, "logs dictionary: size %u\n",
				header.data_length + header.data_offset);
			fprintf(stdout, "including fw version of size: %lu\n\n",
				(unsigned long)sizeof(header.version));
		}
	}
out:
	if (buffer)
		free(buffer);

	return ret;
}

const struct adsp machine_byt = {
	.name = "byt",
	.mem_zones = {
		[SOF_FW_BLK_TYPE_IRAM] = {
			.base = BYT_IRAM_BASE,
			.size = BYT_IRAM_SIZE,
			.host_offset = BYT_IRAM_HOST_OFFSET,
		},
		[SOF_FW_BLK_TYPE_DRAM] = {
			.base = BYT_DRAM_BASE,
			.size = BYT_DRAM_SIZE,
			.host_offset = BYT_DRAM_HOST_OFFSET,
		},
	},
	.machine_id = MACHINE_BAYTRAIL,
	.write_firmware = simple_write_firmware,
};

const struct adsp machine_cht = {
	.name = "cht",
	.mem_zones = {
		[SOF_FW_BLK_TYPE_IRAM] = {
			.base = BYT_IRAM_BASE,
			.size = BYT_IRAM_SIZE,
			.host_offset = BYT_IRAM_HOST_OFFSET,
		},
		[SOF_FW_BLK_TYPE_DRAM] = {
			.base = BYT_DRAM_BASE,
			.size = BYT_DRAM_SIZE,
			.host_offset = BYT_DRAM_HOST_OFFSET,
		},
	},
	.machine_id = MACHINE_CHERRYTRAIL,
	.write_firmware = simple_write_firmware,
};

const struct adsp machine_bsw = {
	.name = "bsw",
	.mem_zones = {
		[SOF_FW_BLK_TYPE_IRAM] = {
			.base = BYT_IRAM_BASE,
			.size = BYT_IRAM_SIZE,
			.host_offset = BYT_IRAM_HOST_OFFSET,
		},
		[SOF_FW_BLK_TYPE_DRAM] = {
			.base = BYT_DRAM_BASE,
			.size = BYT_DRAM_SIZE,
			.host_offset = BYT_DRAM_HOST_OFFSET,
		},
	},
	.machine_id = MACHINE_BRASWELL,
	.write_firmware = simple_write_firmware,
};

const struct adsp machine_hsw = {
	.name = "hsw",
	.mem_zones = {
		[SOF_FW_BLK_TYPE_IRAM] = {
			.base = HSW_IRAM_BASE,
			.size = HSW_IRAM_SIZE,
			.host_offset = HSW_IRAM_HOST_OFFSET,
		},
		[SOF_FW_BLK_TYPE_DRAM] = {
			.base = HSW_DRAM_BASE,
			.size = HSW_DRAM_SIZE,
			.host_offset = HSW_DRAM_HOST_OFFSET,
		},
	},
	.machine_id = MACHINE_HASWELL,
	.write_firmware = simple_write_firmware,
};

const struct adsp machine_bdw = {
	.name = "bdw",
	.mem_zones = {
		[SOF_FW_BLK_TYPE_IRAM] = {
			.base = BDW_IRAM_BASE,
			.size = BDW_IRAM_SIZE,
			.host_offset = BDW_IRAM_HOST_OFFSET,
		},
		[SOF_FW_BLK_TYPE_DRAM] = {
			.base = BDW_DRAM_BASE,
			.size = BDW_DRAM_SIZE,
			.host_offset = BDW_DRAM_HOST_OFFSET,
		},
	},
	.machine_id = MACHINE_BROADWELL,
	.write_firmware = simple_write_firmware,
};

const struct adsp machine_hky = {
	.name = "hky",
	.mem_zones = {
		[SOF_FW_BLK_TYPE_IRAM] = {
			.base = HKY_IRAM_BASE,
			.size = HKY_IRAM_SIZE,
			.host_offset = HKY_IRAM_HOST_OFFSET,
		},
		[SOF_FW_BLK_TYPE_DRAM] = {
			.base = HKY_DRAM_BASE,
			.size = HKY_DRAM_SIZE,
			.host_offset = HKY_DRAM_HOST_OFFSET,
		},
	},
	.machine_id = MACHINE_HIKEY,
	.write_firmware = simple_write_firmware,
};
