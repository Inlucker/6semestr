#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xe09a23a6, "module_layout" },
	{ 0x5a68f739, "proc_symlink" },
	{ 0x31eedeb4, "proc_create" },
	{ 0x1b8ef21c, "proc_mkdir" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x999e8297, "vfree" },
	{ 0x1aea760b, "remove_proc_entry" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x56470118, "__warn_printk" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "436149024D27CB8FA50D136");
