/* =============================
 * Key Symbol Translation Header
 * =============================
 * Taken, in modified form, from: https://github.com/enaudon/abide/
 * 
 * Description:
 * -----------
 *      This header files contains a variety of translation functions (one for
 * each used key symbol type), along with a wrapper function (xlate_ksym) that
 * can translate any key symbol.  The type-specific translation functions
 * should not be directly invoked.  The wrapper function will select the
 * appropriate function to translate a given keystroke.  The wrapper takes two
 * parameters, a pointer to a keyboard_notifier_param and a pointer to an
 * character buffer where the keystroke's string representation should be
 * stored:
 *    void xlate_keysym(keyboard_notifier_param *, char *)
 *
 * Also, note that this file contains a typedef of the keyboard_notifier_param
 * struct to make it easier to deal with; you can call it keystroke_data
 * instead if you like.
 *
 * The keyboard_notifier_param Structure:
 * -------------------------------------
 *      They keyboard_notifier_param structure and some of its fields are a
 * little confusing and deserve some mention.  According to LXR, the
 * keyboard_notifier_param struct is defined as follows in linux v2.6.38-8:
 *
 *    struct keyboard_notifier_param {
 *      struct vc_data *vc;     // VC on which the keyboard press was done
 *      int down;               // Pressure of the key?
 *      int shift;              // Current shift mask
 *      int ledstate;           // Current led state
 *      unsigned int value;     // keycode, unicode value or keysym
 *    };
 *
 * The important fields for our purposes are...
 *    1) down: flag that indicates the pressure of the keyboard event
 *        - zero     = key-press
 *        - non-zero = key-release
 *    2) ledstate: set of binary flags indicating the state of scoll and number
 *                 lock 
 *        - set     = scroll/num lock enabled
 *        - cleared = scroll/num lock disabled
 *        - bit 0: scroll lock
 *        - bit 1: num lock
 *    3) value: (arbitrary) short used to identify each key
 *        - interpretation depends on keyboard mode (we assume non-unicode)
 *        - low-order byte is a value; high-order byte is a type:
 *                val----------------------------+
 *                type--------------------+      |
 *                                        |      |
 *                ks->value = 0x1234 = | 0x12 | 0x23 |
 *        - type used to select the appropriate translation function
 *        - value used to perform the actual translation
 */

#ifndef _KEYSYM_H_
#define _KEYSYM_H_

#include <linux/keyboard.h>

//maximum output buffer size
#define BUFLEN 65535

//ledstate bitmasks
#define SLOCK_MASK 0x00000001   //bit 0 = scroll lock
#define NLOCK_MASK 0x00000002   //bit 1 = num lock

//shift bitmasks
#define CLOCK_MASK 0x00000040   //bit 6 = caps lock

//string representations for common keys
#define UNKNOWN "<unkn>"
#define NO_EFCT "<null>"
#define MENU    "<menu>"
#define ENTER   "<ent>"
#define INSERT  "<ins>"
#define DELETE  "<del>"
#define PAGE_UP "<pg up>"
#define PAGE_DN "<pg dn>"
#define HOME    "<home>"
#define END     "<end>"
#define PAU_BRK "<pb>"
#define ARW_UP  "<u arw>"
#define ARW_DN  "<d arw>"
#define ARW_LT  "<l arw>"
#define ARW_RT  "<r arw>"
#define F_KEYS  "<f"  //no, i didn't forget the end
#define CAPLOCK "<cap _>"

//string representations for key pressure
#define PRESS   'p'
#define RELEASE 'r'

//string representations for (cap/num/scroll) lock status
#define ENABLE  'e'
#define DISABLE 'd'

typedef struct keyboard_notifier_param keystroke_data;

/*Type-specific translation function prototypes.
 *
 *Parameters:
 *  ks   - keystroke_data (keyboard_notifier_param)
 *  buf  - string representation output buffer
 *
 *Returns:
 *  none - result in output buffer
 */
static void ksym_std(keystroke_data *ks, char *buf);
static void ksym_fnc(keystroke_data *ks, char *buf);
static void ksym_loc(keystroke_data *ks, char *buf);
static void ksym_arw(keystroke_data *ks, char *buf);
static void ksym_num(keystroke_data *ks, char *buf);
static void ksym_mod(keystroke_data *ks, char *buf);
static void ksym_cap(keystroke_data *ks, char *buf);

//key symbol maps
//the ascii table could be enlarged to support an extended character set
char *ascii[128] = {
  NO_EFCT, "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "<BEL>",
  "<BS>",  "<TAB>", "<LF>",  "<VT>",  "<FF>",  "<CR>",  "<SO>",  "<SI>",
  "<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>",
  "<CAN>", "<EM>",  "<SUB>", "<ESC>", "<FS>",  "<GS>",  "<RS>",  "<US>",
  " ","!","\"","#","$","%","&","'","(",")","*","+",",", "-",".","/",
  "0","1","2", "3","4","5","6","7","8","9",":",";","<", "=",">","?",
  "@","A","B", "C","D","E","F","G","H","I","J","K","L", "M","N","O",
  "P","Q","R", "S","T","U","V","W","X","Y","Z","[","\\","]","^","_",
  "`","a","b", "c","d","e","f","g","h","i","j","k","l", "m","n","o",
  "p","q","r", "s","t","u","v","w","x","y","z","{","|", "}","~","<DEL>"};
char *fncs[16] = {UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
                  HOME, INSERT, DELETE, END,
                  PAGE_UP, PAGE_DN, UNKNOWN, UNKNOWN,
                  UNKNOWN, PAU_BRK, UNKNOWN, UNKNOWN};
char *locks[16] = {MENU, ENTER, UNKNOWN, UNKNOWN,
                  UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
                  "<num _>", "<scl _>", UNKNOWN, UNKNOWN,
                  UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN};
char *locked_nums[17] = {"0", "1", "2", "3",
                         "4", "5", "6", "7",
                         "8", "9", "+", "-",
                         "*", "/", ENTER, UNKNOWN,
                         "."};
char *unlocked_nums[17] = {INSERT, END, ARW_DN, PAGE_DN,
                           ARW_LT, NO_EFCT, ARW_RT, HOME,
                           ARW_UP, PAGE_UP, "+", "-",
                           "*", "/", ENTER, UNKNOWN,
                           DELETE};
char *arws[4] = {ARW_DN, ARW_LT, ARW_RT, ARW_UP};
char *mods[4] = {"<shift _>", UNKNOWN, "<ctrl _>", "<alt _>"};

/*Wrapper for key symbol translation fucntions.
 *Parses type and selects the appropriate translation function.
 *
 *Parameters:
 *  ks_param - keystroke data
 *  buf      - output buffer
 *
 *Returns:
 *  ni nada
 */
void xlate_keysym(keystroke_data *ks_param, char *buf);

/*Translates "normal" keys--that is, key's that produce ascii.
 *Index into array to get string representations of ascii characters on
 *key-press events.  For printable characters, that's just the character
 *itself; for control characters, that's an abreviation of their function.
 *Key-release events are ignored.
 *
 *NOTE: For printable ascii characters I could have just returned the key
 *      symbol value cast as a character.  However, I opted for a large table
 *      because it can also handle control values, and is more easily modified
 *      to support an extended ascii character set.
 */
static void ksym_std(keystroke_data *ks, char *buf);

/*Translates f-keys and the keys typically above the arrow buttons.
 *If the high nybble of the keyboard_notifier_param's value field (ks->value)
 *is zero'd, we're dealing with an f-key; otherwise we're dealing with one of
 *the keys above the arrow pad.  F-keys are handled by inserting the low nybble
 *of the value field, incremented by 1, into a predefined string. The low
 *nybble+1 is the f-key's number. For example:
 *
 *                          +-----------------type
 *                          |      +----------val
 *                          |      |
 *  ks->value = 0xf103 = | 0xf1 | 0x03 |
 *                                 |
 *                +----------------+
 *                |
 *  buf = "<f" + val+1 + ">" = "<f4>"
 *
 *Non-f-keys are handled by using the low nybble of the value field to index
 *into an array of string represenations.
 *
 *NOTE: f-keys are often handled strangely, so logging results may vary.
 */
static void ksym_fnc(keystroke_data *ks, char *buf);

/*Translate keysymbols with a value of 0x02.
 *The num and scroll lock keys are included here, hence the name of the method.
 *All translation is done by indexing into a character string array, as per
 *usual.  For num and scroll lock though, a status indicator is inserted into
 *the resultant string.
 */
static void ksym_loc(keystroke_data *ks, char *buf);

/*Translates number pad keys.
 *The ledstate field of the keyboard_notifier_param must be parsed to determine
 *whether numlock is enabled or not.  See header description for more info on
 *how ledstate is handled.
 */
static void ksym_num(keystroke_data *ks, char *buf);

/*Translates arrow pad keys.
 *Index into the arrows character string array to obtain the appropriate string
 *for a given arrow key.  Key release events are ignored.
 */
static void ksym_arw(keystroke_data *ks, char *buf);

/*Translates normal modifier keys.
 *Appends the appropriate string to the output buffer on both key-press and
 *key-release events.  An event-specific character is inserted into the string
 *to indicate key pressure (p for press, r for release events).
 *
 *NOTE: The alt and meta keys both produce the same key symbol, and are
 *      therefore indistinguishable at this point in the line discipline.  We
 *      could have captured keycodes instead (each key has a completely unique
 *      keycode), but the translation process would have been more complex.
 */
static void ksym_mod(keystroke_data *ks, char *buf);

/*Translates the capslock key.
 *Appends the appropriate string to the output buffer on key-release events.  
 *(Here we ignore key-press events because the caps lock state doesn't change
 *until after the caps lock key is pressed.)  An event-specific character is
 *inserted into the string to indicate the NEW capslock state (e for enabled,
 *d for disabled).
 */
static void ksym_cap (keystroke_data *ks, char *buf);

#endif