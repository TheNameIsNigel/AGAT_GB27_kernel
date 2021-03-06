/*
 * Copyright (C) 2010 Samsung Electronics. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifdef CONFIG_SEC_MODEM_GODIVA2
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>

/* inlcude platform specific file */
#include <linux/cpufreq_pegasusq.h>
#include <linux/platform_data/modem.h>

#include <plat/gpio-cfg.h>
#include <plat/regs-srom.h>
#include <plat/devs.h>
#include <plat/ehci.h>

#include <mach/dev.h>
#include <mach/gpio.h>
#include <mach/gpio-exynos4.h>
#include <mach/regs-mem.h>
#include <mach/cpufreq.h>
#include <mach/c2c.h>
#include <mach/sromc-exynos4.h>

#ifdef CONFIG_UMTS_MODEM_SS222
static struct modem_io_t umts_io_devices[] = {
	[0] = {
		.name = "umts_boot0",
		.id = 0,
		.format = IPC_BOOT,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "CBD"
	},
	[1] = {
		.name = "umts_ipc0",
		.id = 235,
		.format = IPC_FMT,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "RIL"
	},
	[2] = {
		.name = "umts_rfs0",
		.id = 245,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "RFS"
	},
	[3] = {
		.name = "multipdp",
		.id = 0,
		.format = IPC_MULTI_RAW,
		.io_type = IODEV_DUMMY,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[4] = {
		.name = "lte_rmnet0",
		.id = 10,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[5] = {
		.name = "lte_rmnet1",
		.id = 11,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[6] = {
		.name = "lte_rmnet2",
		.id = 12,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[7] = {
		.name = "lte_rmnet3",
		.id = 13,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[8] = {
		.name = "umts_csd",	/* CS Video Telephony */
		.id = 1,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[9] = {
		.name = "umts_router",	/* AT Iface & Dial-up */
		.id = 25,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "Data Router"
	},
	[10] = {
		.name = "umts_dm0",	/* DM Port */
		.id = 28,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "DIAG"
	},
	[11] = {
		.name = "umts_loopback_cp2ap",
		.id = 30,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "CP Loopback"
	},
	[12] = {
		.name = "umts_loopback_ap2cp",
		.id = 31,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
		.app = "AP loopback"
	},
	[13] = {
		.name = "umts_ramdump0",
		.id = 0,
		.format = IPC_RAMDUMP,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
	},
	[14] = {
		.name = "umts_log",
		.id = 0,
		.format = IPC_RAMDUMP,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_C2C),
	},
};
#endif

#ifdef CONFIG_CDMA_MODEM_CBP82
static struct modem_io_t cdma_io_devices[] = {
	[0] = {
		.name = "cdma_boot0",
		.id = 0,
		.format = IPC_BOOT,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
		.app = "CBD"
	},
	[1] = {
		.name = "cdma_ipc0",
		.id = 235,
		.format = IPC_FMT,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
		.app = "RIL"
	},
	[2] = {
		.name = "cdma_rfs0",
		.id = 245,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
		.app = "RFS"
	},
	[3] = {
		.name = "cdma_multipdp",
		.id = 0,
		.format = IPC_MULTI_RAW,
		.io_type = IODEV_DUMMY,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[4] = {
		.name = "cdma_rmnet0",
		.id = 10,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[5] = {
		.name = "cdma_rmnet1",
		.id = 11,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[6] = {
		.name = "cdma_rmnet2",
		.id = 12,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[7] = {
		.name = "cdma_rmnet3",
		.id = 13,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[8] = {
		.name = "cdma_rmnet4",
		.id = 7,
		.format = IPC_RAW,
		.io_type = IODEV_NET,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[9] = {
		.name = "cdma_rmnet5", /* DM Port IO device */
		.id = 26,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[10] = {
		.name = "cdma_rmnet6", /* AT CMD IO device */
		.id = 17,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[11] = {
		.name = "cdma_ramdump0",
		.id = 0,
		.format = IPC_RAMDUMP,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
	},
	[12] = {
		.name = "cdma_cplog",
		.id = 29,
		.format = IPC_RAW,
		.io_type = IODEV_MISC,
		.links = LINKTYPE(LINKDEV_DPRAM),
		.app = "DIAG"
	},
};
#endif

#ifdef CONFIG_UMTS_MODEM_SS222
static struct resource umts_modem_res[] = {
	[0] = {
		.name = STR_SHMEM_BASE,
		.start = C2C_SH_RGN_BASE,
		.end = C2C_SH_RGN_BASE + (C2C_SH_RGN_SIZE - 1),
		.flags = IORESOURCE_MEM,
	},
};

static struct modem_data umts_modem_data = {
	.name = "ss222",

	.gpio_cp_on = GPIO_LTE_PMIC_PWRON,
	.gpio_cp_reset = GPIO_LTE_CP_RESET,
	.gpio_cp_off = GPIO_LTE_PS_HOLD_OFF,

	.gpio_pda_active = GPIO_PDA_ACTIVE,

	.gpio_phone_active = GPIO_LTE_ACTIVE,
	.irq_phone_active = IRQ_LTE_ACTIVE,

	.gpio_ap_wakeup = GPIO_LTE2AP_WAKEUP,
	.irq_ap_wakeup = IRQ_LTE2AP_WAKEUP,
	.gpio_ap_status = GPIO_AP2LTE_STATUS,

	.gpio_cp_wakeup = GPIO_AP2LTE_WAKEUP,
	.gpio_cp_status = GPIO_LTE2AP_STATUS,
	.irq_cp_status = IRQ_LTE2AP_STATUS,

	.modem_net = UMTS_NETWORK,
	.modem_type = SEC_SS222,
	.link_types = LINKTYPE(LINKDEV_C2C),
	.link_name = "c2c",

	.num_iodevs = ARRAY_SIZE(umts_io_devices),
	.iodevs = umts_io_devices,

	.use_handover = true,

	.ipc_version = SIPC_VER_50,
};

static struct platform_device umts_modem = {
	.name = "mif_sipc5",
	.id = 1,
	.num_resources = ARRAY_SIZE(umts_modem_res),
	.resource = umts_modem_res,
	.dev = {
		.platform_data = &umts_modem_data,
	},
};

/*
** Function definitions
*/
static void config_umts_modem_gpio(void)
{
	int err;
	unsigned gpio_cp_on = umts_modem_data.gpio_cp_on;
	unsigned gpio_cp_off = umts_modem_data.gpio_cp_off;
	unsigned gpio_cp_rst = umts_modem_data.gpio_cp_reset;
	unsigned gpio_pda_active = umts_modem_data.gpio_pda_active;
	unsigned gpio_phone_active = umts_modem_data.gpio_phone_active;
	unsigned gpio_ap_wakeup = umts_modem_data.gpio_ap_wakeup;
	unsigned gpio_ap_status = umts_modem_data.gpio_ap_status;
	unsigned gpio_cp_wakeup = umts_modem_data.gpio_cp_wakeup;
	unsigned gpio_cp_status = umts_modem_data.gpio_cp_status;
	mif_err("+++\n");

	if (gpio_cp_on) {
		err = gpio_request(gpio_cp_on, "LTE_ON");
		if (err) {
			mif_err("ERR! LTE_ON gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_on, 0);
			s3c_gpio_setpull(gpio_cp_on, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_cp_off) {
		err = gpio_request(gpio_cp_off, "LTE_OFF");
		if (err) {
			mif_err("ERR! LTE_OFF gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_off, 0);
			s3c_gpio_setpull(gpio_cp_off, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_cp_rst) {
		err = gpio_request(gpio_cp_rst, "LTE_RST");
		if (err) {
			mif_err("ERR! LTE_RST gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_rst, 0);
			s3c_gpio_setpull(gpio_cp_rst, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_pda_active) {
		err = gpio_request(gpio_pda_active, "PDA_ACTIVE");
		if (err) {
			mif_err("ERR! PDA_ACTIVE gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_pda_active, 1);
			s3c_gpio_setpull(gpio_pda_active, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_phone_active) {
		err = gpio_request(gpio_phone_active, "LTE_ACTIVE");
		if (err) {
			mif_err("ERR! LTE_ACTIVE gpio_request fail\n");
		} else {
			/* Configure as a wake-up source */
			gpio_direction_input(gpio_phone_active);
			s3c_gpio_setpull(gpio_phone_active, S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(gpio_phone_active, S3C_GPIO_SFN(0xF));
		}
	}

	if (gpio_ap_wakeup) {
		err = gpio_request(gpio_ap_wakeup, "LTE2AP_WAKEUP");
		if (err) {
			mif_err("ERR! LTE2AP_WAKEUP gpio_request fail\n");
		} else {
			/* Configure as a wake-up source */
			gpio_direction_input(gpio_ap_wakeup);
			s3c_gpio_setpull(gpio_ap_wakeup, S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(gpio_ap_wakeup, S3C_GPIO_SFN(0xF));
		}
	}

	if (gpio_ap_status) {
		err = gpio_request(gpio_ap_status, "AP2LTE_STATUS");
		if (err) {
			mif_err("ERR! AP2LTE_STATUS gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_ap_status, 0);
			s3c_gpio_setpull(gpio_ap_status, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_cp_wakeup) {
		err = gpio_request(gpio_cp_wakeup, "AP2LTE_WAKEUP");
		if (err) {
			mif_err("ERR! AP2LTE_WAKEUP gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_wakeup, 0);
			s3c_gpio_setpull(gpio_cp_wakeup, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_cp_status) {
		err = gpio_request(gpio_cp_status, "LTE2AP_STATUS");
		if (err) {
			mif_err("ERR! LTE2AP_STATUS gpio_request fail\n");
		} else {
			gpio_direction_input(gpio_cp_status);
			s3c_gpio_setpull(gpio_cp_status, S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(gpio_ap_wakeup, S3C_GPIO_SFN(0xF));
		}
	}

	mif_err("---\n");
}
#endif

#ifdef CONFIG_CDMA_MODEM_CBP82
/* For CBP7.2 EDPRAM (External DPRAM) */
#define CBP_EDPRAM_BASE		SROM_CS0_BASE
#define CBP_EDPRAM_SIZE		DPRAM_SIZE_16KB

/*
** addr_bits: 13 bits for CBP82
** data_bits: 16 bits
** byte_acc : true for CBP82
*/
static struct sromc_bus_cfg godiva2_sromc_bus_cfg = {
	.addr_bits = 13,
	.data_bits = 16,
	.byte_acc = 1,
};

static struct sromc_bank_cfg cbp_edpram_bank_cfg = {
	.csn = 0,
	.attr = SROMC_DATA_16 | SROMC_BYTE_EN,
};

static struct sromc_timing_cfg cbp_edpram_timing_cfg = {
	.tacs = 0x00 << 28,
	.tcos = 0x00 << 24,
	.tacc = 0x0F << 16,
	.tcoh = 0x00 << 12,
	.tcah = 0x00 << 8,
	.tacp = 0x00 << 4,
	.pmc  = 0x00 << 0,
};

static struct modemlink_dpram_data cbp_edpram = {
	.type = EXT_DPRAM,
	.res_ack_wait_timeout = 100,
};

static struct resource cdma_modem_res[] = {
	[RES_DPRAM_MEM_ID] = {
		.name = STR_DPRAM_BASE,
		.start = CBP_EDPRAM_BASE,
		.end = CBP_EDPRAM_BASE + (CBP_EDPRAM_SIZE - 1),
		.flags = IORESOURCE_MEM,
	},
};

static struct modem_data cdma_modem_data = {
	.name = "cbp8.2",

	.gpio_cp_on = GPIO_VIA_PMIC_PWRON,
	.gpio_cp_off = GPIO_VIA_PS_HOLD_OFF,
	.gpio_cp_reset = GPIO_VIA_CP_RESET,

	.gpio_pda_active = GPIO_PDA_ACTIVE,

	.gpio_phone_active = GPIO_VIA_ACTIVE,
	.irq_phone_active = IRQ_VIA_ACTIVE,

	.gpio_ipc_int2ap = GPIO_VIA_DPRAM_INT,
	.irq_ipc_int2ap = IRQ_VIA_DPRAM_INT,
	.irqf_ipc_int2ap = (IRQF_NO_SUSPEND | IRQF_TRIGGER_FALLING),

	.modem_net = CDMA_NETWORK,
	.modem_type = VIA_CBP82,
	.link_types = LINKTYPE(LINKDEV_DPRAM),
	.link_name = "cbp82_edpram",
	.dpram = &cbp_edpram,

	.num_iodevs = ARRAY_SIZE(cdma_io_devices),
	.iodevs = cdma_io_devices,

	.use_handover = true,

	.ipc_version = SIPC_VER_50,
};

static struct platform_device cdma_modem = {
	.name = "mif_sipc5",
	.id = 2,
	.num_resources = ARRAY_SIZE(cdma_modem_res),
	.resource = cdma_modem_res,
	.dev = {
		.platform_data = &cdma_modem_data,
	},
};

static void config_cdma_modem_gpio(void)
{
	int err;
	unsigned gpio_cp_on = cdma_modem_data.gpio_cp_on;
	unsigned gpio_cp_off = cdma_modem_data.gpio_cp_off;
	unsigned gpio_cp_rst = cdma_modem_data.gpio_cp_reset;
	unsigned gpio_pda_active = cdma_modem_data.gpio_pda_active;
	unsigned gpio_phone_active = cdma_modem_data.gpio_phone_active;
	unsigned gpio_ipc_int2ap = cdma_modem_data.gpio_ipc_int2ap;
	mif_err("+++\n");

	if (gpio_cp_on) {
		err = gpio_request(gpio_cp_on, "VIA_ON");
		if (err) {
			mif_err("ERR! VIA_ON gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_on, 0);
			s3c_gpio_setpull(gpio_cp_on, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_cp_off) {
		err = gpio_request(gpio_cp_off, "VIA_OFF");
		if (err) {
			mif_err("ERR! VIA_OFF gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_off, 1);
			s3c_gpio_setpull(gpio_cp_off, S3C_GPIO_PULL_NONE);
		}
	}

	if (gpio_cp_rst) {
		err = gpio_request(gpio_cp_rst, "VIA_RST");
		if (err) {
			mif_err("ERR! VIA_RST gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_cp_rst, 0);
			s3c_gpio_setpull(gpio_cp_rst, S3C_GPIO_PULL_NONE);
		}
	}

#ifndef CONFIG_UMTS_MODEM_SS222
	if (gpio_pda_active) {
		err = gpio_request(gpio_pda_active, "PDA_ACTIVE");
		if (err) {
			mif_err("ERR! PDA_ACTIVE gpio_request fail\n");
		} else {
			gpio_direction_output(gpio_pda_active, 0);
			s3c_gpio_setpull(gpio_pda_active, S3C_GPIO_PULL_NONE);
		}
	}
#endif

	if (gpio_phone_active) {
		err = gpio_request(gpio_phone_active, "VIA_ACTIVE");
		if (err) {
			mif_err("ERR! VIA_ACTIVE gpio_request fail\n");
		} else {
			/* Configure as a wake-up source */
			gpio_direction_input(gpio_phone_active);
			s3c_gpio_setpull(gpio_phone_active, S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(gpio_phone_active, S3C_GPIO_SFN(0xF));
		}
	}

	if (gpio_ipc_int2ap) {
		err = gpio_request(gpio_ipc_int2ap, "VIA_DPRAM_INT");
		if (err) {
			mif_err("ERR! VIA_DPRAM_INT gpio_request fail\n");
		} else {
			/* Configure as a wake-up source */
			gpio_direction_input(gpio_ipc_int2ap);
			s3c_gpio_setpull(gpio_ipc_int2ap, S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(gpio_ipc_int2ap, S3C_GPIO_SFN(0xF));
		}
	}

	/* set low unused gpios between AP and CP */
	err = gpio_request(GPIO_UART_AP_VIA_RXD, "AP_VIA_RXD");
	if (err) {
		mif_err("ERR! AP_VIA_RXD gpio_request fail\n");
	} else {
		gpio_direction_input(GPIO_UART_AP_VIA_RXD);
		s3c_gpio_setpull(GPIO_UART_AP_VIA_RXD, S3C_GPIO_PULL_UP);
		s3c_gpio_cfgpin(GPIO_UART_AP_VIA_RXD, GPIO_UART_SFN);
	}

	err = gpio_request(GPIO_UART_AP_VIA_TXD, "AP_VIA_TXD");
	if (err) {
		mif_err("ERR! AP_VIA_TXD gpio_request fail\n");
	} else {
		gpio_direction_output(GPIO_UART_AP_VIA_TXD, 0);
		s3c_gpio_setpull(GPIO_UART_AP_VIA_TXD, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(GPIO_UART_AP_VIA_TXD, GPIO_UART_SFN);
	}

	mif_err("---\n");
}
#endif

static int __init init_modem(void)
{
	struct sromc_bus_cfg *bus_cfg;
	struct sromc_bank_cfg *bnk_cfg;
	struct sromc_timing_cfg *tm_cfg;
	int err;
	unsigned size;
	mif_err("+++\n");

	mif_err("System Revision = %d\n", system_rev);

	/*
	** Configure GPIO pins for the modem
	*/
#ifdef CONFIG_UMTS_MODEM_SS222
	config_umts_modem_gpio();
#endif
#ifdef CONFIG_CDMA_MODEM_CBP82
	config_cdma_modem_gpio();
#endif

#ifdef CONFIG_CDMA_MODEM_CBP82
	/*
	** Configure SROM controller
	*/
	err = sromc_enable();
	if (err < 0) {
		mif_err("ERR! sromc_enable fail (err %d)\n", err);
		goto error;
	}

	bus_cfg = &godiva2_sromc_bus_cfg;
	err = sromc_config_demux_gpio(bus_cfg);
	if (err < 0) {
		mif_err("ERR! sromc_config_demux_gpio fail (err %d)\n", err);
		goto error;
	}

	bnk_cfg = &cbp_edpram_bank_cfg;
	err = sromc_config_csn_gpio(bnk_cfg->csn);
	if (err < 0) {
		mif_err("ERR! sromc_config_csn_gpio fail (err %d)\n", err);
		goto error;
	}
	sromc_config_access_attr(bnk_cfg->csn, bnk_cfg->attr);

	tm_cfg = &cbp_edpram_timing_cfg;
	sromc_config_access_timing(bnk_cfg->csn, tm_cfg);
#endif

	/*
	** Register the modem devices
	*/
#ifdef CONFIG_UMTS_MODEM_SS222
	err = platform_device_register(&umts_modem);
	if (err) {
		mif_err("%s: %s: ERR! platform_device_register fail (err %d)\n",
			umts_modem_data.name, umts_modem.name, err);
		goto error;
	}
#endif
#ifdef CONFIG_CDMA_MODEM_CBP82
	err = platform_device_register(&cdma_modem);
	if (err) {
		mif_err("%s: %s: ERR! platform_device_register fail (err %d)\n",
			cdma_modem_data.name, cdma_modem.name, err);
		goto error;
	}
#endif

	mif_err("---\n");
	return 0;

error:
	mif_err("xxx\n");
	return err;
}
late_initcall(init_modem);

#endif /*CONFIG_SEC_MODEM_GODIVA2*/

