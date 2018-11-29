static const uint32_t to_ucs4[256] = {
  [0x01] = 0x0001,
  [0x02] = 0x0002,
  [0x03] = 0x0003,
  [0x04] = 0x0004,
  [0x05] = 0x0005,
  [0x06] = 0x0006,
  [0x07] = 0x0007,
  [0x08] = 0x0008,
  [0x09] = 0x0009,
  [0x0a] = 0x000A,
  [0x0b] = 0x000B,
  [0x0c] = 0x000C,
  [0x0d] = 0x000D,
  [0x0e] = 0x000E,
  [0x0f] = 0x000F,
  [0x10] = 0x0010,
  [0x11] = 0x0011,
  [0x12] = 0x0012,
  [0x13] = 0x0013,
  [0x14] = 0x0014,
  [0x15] = 0x0015,
  [0x16] = 0x0016,
  [0x17] = 0x0017,
  [0x18] = 0x0018,
  [0x19] = 0x0019,
  [0x1a] = 0x001A,
  [0x1b] = 0x001B,
  [0x1c] = 0x001C,
  [0x1d] = 0x001D,
  [0x1e] = 0x001E,
  [0x1f] = 0x001F,
  [0x20] = 0x0020,
  [0x21] = 0x0021,
  [0x22] = 0x0022,
  [0x23] = 0x0023,
  [0x24] = 0x0024,
  [0x25] = 0x0025,
  [0x26] = 0x0026,
  [0x27] = 0x0027,
  [0x28] = 0x0028,
  [0x29] = 0x0029,
  [0x2a] = 0x002A,
  [0x2b] = 0x002B,
  [0x2c] = 0x002C,
  [0x2d] = 0x002D,
  [0x2e] = 0x002E,
  [0x2f] = 0x002F,
  [0x30] = 0x0030,
  [0x31] = 0x0031,
  [0x32] = 0x0032,
  [0x33] = 0x0033,
  [0x34] = 0x0034,
  [0x35] = 0x0035,
  [0x36] = 0x0036,
  [0x37] = 0x0037,
  [0x38] = 0x0038,
  [0x39] = 0x0039,
  [0x3a] = 0x003A,
  [0x3b] = 0x003B,
  [0x3c] = 0x003C,
  [0x3d] = 0x003D,
  [0x3e] = 0x003E,
  [0x3f] = 0x003F,
  [0x40] = 0x0040,
  [0x41] = 0x0041,
  [0x42] = 0x0042,
  [0x43] = 0x0043,
  [0x44] = 0x0044,
  [0x45] = 0x0045,
  [0x46] = 0x0046,
  [0x47] = 0x0047,
  [0x48] = 0x0048,
  [0x49] = 0x0049,
  [0x4a] = 0x004A,
  [0x4b] = 0x004B,
  [0x4c] = 0x004C,
  [0x4d] = 0x004D,
  [0x4e] = 0x004E,
  [0x4f] = 0x004F,
  [0x50] = 0x0050,
  [0x51] = 0x0051,
  [0x52] = 0x0052,
  [0x53] = 0x0053,
  [0x54] = 0x0054,
  [0x55] = 0x0055,
  [0x56] = 0x0056,
  [0x57] = 0x0057,
  [0x58] = 0x0058,
  [0x59] = 0x0059,
  [0x5a] = 0x005A,
  [0x5b] = 0x005B,
  [0x5c] = 0x005C,
  [0x5d] = 0x005D,
  [0x5e] = 0x005E,
  [0x5f] = 0x005F,
  [0x60] = 0x0060,
  [0x61] = 0x0061,
  [0x62] = 0x0062,
  [0x63] = 0x0063,
  [0x64] = 0x0064,
  [0x65] = 0x0065,
  [0x66] = 0x0066,
  [0x67] = 0x0067,
  [0x68] = 0x0068,
  [0x69] = 0x0069,
  [0x6a] = 0x006A,
  [0x6b] = 0x006B,
  [0x6c] = 0x006C,
  [0x6d] = 0x006D,
  [0x6e] = 0x006E,
  [0x6f] = 0x006F,
  [0x70] = 0x0070,
  [0x71] = 0x0071,
  [0x72] = 0x0072,
  [0x73] = 0x0073,
  [0x74] = 0x0074,
  [0x75] = 0x0075,
  [0x76] = 0x0076,
  [0x77] = 0x0077,
  [0x78] = 0x0078,
  [0x79] = 0x0079,
  [0x7a] = 0x007A,
  [0x7b] = 0x007B,
  [0x7c] = 0x007C,
  [0x7d] = 0x007D,
  [0x7e] = 0x007E,
  [0x7f] = 0x007F,
  [0x80] = 0x20AC,
  [0x85] = 0x2026,
  [0x91] = 0x2018,
  [0x92] = 0x2019,
  [0x93] = 0x201C,
  [0x94] = 0x201D,
  [0x95] = 0x2022,
  [0x96] = 0x2013,
  [0x97] = 0x2014,
  [0xa0] = 0x00A0,
  [0xa1] = 0x0E01,
  [0xa2] = 0x0E02,
  [0xa3] = 0x0E03,
  [0xa4] = 0x0E04,
  [0xa5] = 0x0E05,
  [0xa6] = 0x0E06,
  [0xa7] = 0x0E07,
  [0xa8] = 0x0E08,
  [0xa9] = 0x0E09,
  [0xaa] = 0x0E0A,
  [0xab] = 0x0E0B,
  [0xac] = 0x0E0C,
  [0xad] = 0x0E0D,
  [0xae] = 0x0E0E,
  [0xaf] = 0x0E0F,
  [0xb0] = 0x0E10,
  [0xb1] = 0x0E11,
  [0xb2] = 0x0E12,
  [0xb3] = 0x0E13,
  [0xb4] = 0x0E14,
  [0xb5] = 0x0E15,
  [0xb6] = 0x0E16,
  [0xb7] = 0x0E17,
  [0xb8] = 0x0E18,
  [0xb9] = 0x0E19,
  [0xba] = 0x0E1A,
  [0xbb] = 0x0E1B,
  [0xbc] = 0x0E1C,
  [0xbd] = 0x0E1D,
  [0xbe] = 0x0E1E,
  [0xbf] = 0x0E1F,
  [0xc0] = 0x0E20,
  [0xc1] = 0x0E21,
  [0xc2] = 0x0E22,
  [0xc3] = 0x0E23,
  [0xc4] = 0x0E24,
  [0xc5] = 0x0E25,
  [0xc6] = 0x0E26,
  [0xc7] = 0x0E27,
  [0xc8] = 0x0E28,
  [0xc9] = 0x0E29,
  [0xca] = 0x0E2A,
  [0xcb] = 0x0E2B,
  [0xcc] = 0x0E2C,
  [0xcd] = 0x0E2D,
  [0xce] = 0x0E2E,
  [0xcf] = 0x0E2F,
  [0xd0] = 0x0E30,
  [0xd1] = 0x0E31,
  [0xd2] = 0x0E32,
  [0xd3] = 0x0E33,
  [0xd4] = 0x0E34,
  [0xd5] = 0x0E35,
  [0xd6] = 0x0E36,
  [0xd7] = 0x0E37,
  [0xd8] = 0x0E38,
  [0xd9] = 0x0E39,
  [0xda] = 0x0E3A,
  [0xdf] = 0x0E3F,
  [0xe0] = 0x0E40,
  [0xe1] = 0x0E41,
  [0xe2] = 0x0E42,
  [0xe3] = 0x0E43,
  [0xe4] = 0x0E44,
  [0xe5] = 0x0E45,
  [0xe6] = 0x0E46,
  [0xe7] = 0x0E47,
  [0xe8] = 0x0E48,
  [0xe9] = 0x0E49,
  [0xea] = 0x0E4A,
  [0xeb] = 0x0E4B,
  [0xec] = 0x0E4C,
  [0xed] = 0x0E4D,
  [0xee] = 0x0E4E,
  [0xef] = 0x0E4F,
  [0xf0] = 0x0E50,
  [0xf1] = 0x0E51,
  [0xf2] = 0x0E52,
  [0xf3] = 0x0E53,
  [0xf4] = 0x0E54,
  [0xf5] = 0x0E55,
  [0xf6] = 0x0E56,
  [0xf7] = 0x0E57,
  [0xf8] = 0x0E58,
  [0xf9] = 0x0E59,
  [0xfa] = 0x0E5A,
  [0xfb] = 0x0E5B,
};
static const struct gap from_idx[] = {
  { .start = 0x0000, .end = 0x007f, .idx =     0 },
  { .start = 0x00a0, .end = 0x00a0, .idx =   -32 },
  { .start = 0x0e01, .end = 0x0e5b, .idx = -3456 },
  { .start = 0x2013, .end = 0x2026, .idx = -7991 },
  { .start = 0x20ac, .end = 0x20ac, .idx = -8124 },
  { .start = 0xffff, .end = 0xffff, .idx =     0 }
};
static const char from_ucs4[] = {

  '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
  '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
  '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
  '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27',
  '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
  '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37',
  '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
  '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47',
  '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
  '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57',
  '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
  '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67',
  '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
  '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77',
  '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7',
  '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7',
  '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7',
  '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7',
  '\xd8', '\xd9', '\xda', '\x00', '\x00', '\x00', '\x00', '\xdf',
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7',
  '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7',
  '\xf8', '\xf9', '\xfa', '\xfb', '\x96', '\x97', '\x00', '\x00',
  '\x00', '\x91', '\x92', '\x00', '\x00', '\x93', '\x94', '\x00',
  '\x00', '\x00', '\x00', '\x95', '\x00', '\x00', '\x00', '\x85',
  '\x80',
};
