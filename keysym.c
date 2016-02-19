#include "keysym.h"

void xlate_keysym(keystroke_data *ks_param, char *buf) {
  unsigned char type = ks_param->value >> 8;

  //high nybble doesn't seem to be useful
  type &= 0x0f;

  //select appropriate translation function
  switch (type) {
    case 0x0 : ksym_std(ks_param, buf);  break;
    case 0x1 : ksym_fnc(ks_param, buf);  break;
    case 0x2 : ksym_loc(ks_param, buf);  break;
    case 0x3 : ksym_num(ks_param, buf);  break;
    case 0x4 : break;
    case 0x5 : break;
    case 0x6 : ksym_arw(ks_param, buf);  break;
    case 0x7 : ksym_mod(ks_param, buf);  break;
    case 0x8 : break;
    case 0x9 : break;
    case 0xa : ksym_cap(ks_param, buf);  break;
    case 0xb : ksym_std(ks_param, buf);  break;
    case 0xc : break;
    case 0xd : break;
    case 0xe : break;
    case 0xf : break;
  }
}

static void ksym_std(keystroke_data *ks, char *buf) {
  unsigned char val  = ks->value & 0x00ff;

  //ignore key-release events
  if (!ks->down) return;

  //otherwise return string representation
  strlcat(buf, ascii[val], BUFLEN);
}

static void ksym_fnc(keystroke_data *ks, char *buf) {
  unsigned char val  = ks->value & 0x00ff;
  char temp[6];

  //ignore key-release events
  if (!ks->down) return;

  //non-f-keys when the high nybble isn't zero'd
  if (val & 0xf0) strlcat(buf, fncs[val&0x0f], BUFLEN);

  //f-key otherwise
  else {
    snprintf(temp, 6, "%s%d>", F_KEYS, ++val);
    strlcat(buf, temp, BUFLEN);
  }
}

static void ksym_loc(keystroke_data *ks, char *buf) {
    int len;
    unsigned char val  = ks->value & 0x00ff;
    int n_lock = ks->ledstate & NLOCK_MASK;
    int s_lock = ks->ledstate & SLOCK_MASK;

    //just in case
    if (val > 16) return;

    //ignore key-press events
    if (ks->down) return;

    //translate key symbol
    len = strlcat(buf, locks[val], BUFLEN);

    //handle status indicator for num and scroll lock, respectively
    if      (val == 0x08) buf[len - 2] = n_lock ? ENABLE : DISABLE;
    else if (val == 0x09) buf[len - 2] = s_lock ? ENABLE : DISABLE;
}

static void ksym_num(keystroke_data *ks, char *buf) {
  unsigned char val  = ks->value & 0x00ff;
  int n_lock = ks->ledstate & NLOCK_MASK;

  //just in case
  if (val > 16) return;

  //ignore key-release events
  if (!ks->down) return;

  //values depend on the state of numlock
  if (n_lock)
    strlcat(buf, locked_nums[val], BUFLEN);
  else if (!n_lock)
    strlcat(buf, unlocked_nums[val], BUFLEN);
}

static void ksym_arw(keystroke_data *ks, char *buf) {
  unsigned char val  = ks->value & 0x00ff;

  //just in case
  if (val > 3) return;

  //ignore key-release events
  if (!ks->down) return;

  //translate arrow key to string
  strlcat(buf, arws[val], BUFLEN);
}

static void ksym_mod(keystroke_data *ks, char *buf) {
  int len;
  unsigned char val  = ks->value & 0x00ff;

  //just in case
  if (val > 3) return;

  //translate mod key to string
  len = strlcat(buf, mods[val], BUFLEN);

  //add pressure indicator
  buf[len - 2] = ks->down ? PRESS : RELEASE;
}

static void ksym_cap (keystroke_data *ks, char *buf) {
  int len;
  unsigned char val  = ks->value & 0x00ff;
  int c_lock = ks->shift & CLOCK_MASK;
 
  //ignore key-press events
  if (ks->down) return;

  if (val == 0x06) {
    //translate mod key to string
    len = strlcat(buf, CAPLOCK, BUFLEN);

    //add lock status indicator
    buf[len - 2] = c_lock ? ENABLE : DISABLE;
  }
  else strlcat(buf, UNKNOWN, BUFLEN);
}


