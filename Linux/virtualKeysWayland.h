#ifndef  _VIRTUALKEYSLINUX_H
#define  _VIRTUALKEYSLINUX_H

#include "../virtualKeysGame.h"

// created based on input-event-codes.h
static GK_VirtualKey waylandVirtualKeysTable[] = {
    /* 0 */   GK_VirtualKey::GK_UNKNOWN,
    /* 1 */   GK_VirtualKey::GK_Escape,
    /* 2 */   GK_VirtualKey::GK_UNKNOWN,
    /* 3 */   GK_VirtualKey::GK_UNKNOWN,
    /* 4 */   GK_VirtualKey::GK_UNKNOWN,
    /* 5 */   GK_VirtualKey::GK_UNKNOWN,
    /* 6 */   GK_VirtualKey::GK_UNKNOWN,
    /* 7 */   GK_VirtualKey::GK_UNKNOWN,
    /* 8 */   GK_VirtualKey::GK_UNKNOWN,
    /* 9 */   GK_VirtualKey::GK_UNKNOWN,
    /* 10 */  GK_VirtualKey::GK_UNKNOWN,
    /* 11 */  GK_VirtualKey::GK_UNKNOWN,
    /* 12 */  GK_VirtualKey::GK_UNKNOWN,
    /* 13 */  GK_VirtualKey::GK_UNKNOWN,
    /* 14 */  GK_VirtualKey::GK_BackSpace,
    /* 15 */  GK_VirtualKey::GK_Tab,
    /* 16 */  GK_VirtualKey::GK_UNKNOWN,
    /* 17 */  GK_VirtualKey::GK_UNKNOWN,
    /* 18 */  GK_VirtualKey::GK_UNKNOWN,
    /* 19 */  GK_VirtualKey::GK_UNKNOWN,
    /* 20 */  GK_VirtualKey::GK_UNKNOWN,
    /* 21 */  GK_VirtualKey::GK_UNKNOWN,
    /* 22 */  GK_VirtualKey::GK_UNKNOWN,
    /* 23 */  GK_VirtualKey::GK_UNKNOWN,
    /* 24 */  GK_VirtualKey::GK_UNKNOWN,
    /* 25 */  GK_VirtualKey::GK_UNKNOWN,
    /* 26 */  GK_VirtualKey::GK_UNKNOWN,
    /* 27 */  GK_VirtualKey::GK_UNKNOWN,
    /* 28 */  GK_VirtualKey::GK_UNKNOWN,
    /* 29 */  GK_VirtualKey::GK_ControlL,
    /* 30 */  GK_VirtualKey::GK_UNKNOWN,
    /* 31 */  GK_VirtualKey::GK_UNKNOWN,
    /* 32 */  GK_VirtualKey::GK_UNKNOWN,
    /* 33 */  GK_VirtualKey::GK_UNKNOWN,
    /* 34 */  GK_VirtualKey::GK_UNKNOWN,
    /* 35 */  GK_VirtualKey::GK_UNKNOWN,
    /* 36 */  GK_VirtualKey::GK_UNKNOWN,
    /* 37 */  GK_VirtualKey::GK_UNKNOWN,
    /* 38 */  GK_VirtualKey::GK_UNKNOWN,
    /* 39 */  GK_VirtualKey::GK_UNKNOWN,
    /* 40 */  GK_VirtualKey::GK_UNKNOWN,
    /* 41 */  GK_VirtualKey::GK_UNKNOWN,
    /* 42 */  GK_VirtualKey::GK_ShiftL,
    /* 43 */  GK_VirtualKey::GK_UNKNOWN,
    /* 44 */  GK_VirtualKey::GK_UNKNOWN,
    /* 45 */  GK_VirtualKey::GK_UNKNOWN,
    /* 46 */  GK_VirtualKey::GK_UNKNOWN,
    /* 47 */  GK_VirtualKey::GK_UNKNOWN,
    /* 48 */  GK_VirtualKey::GK_UNKNOWN,
    /* 49 */  GK_VirtualKey::GK_UNKNOWN,
    /* 50 */  GK_VirtualKey::GK_UNKNOWN,
    /* 51 */  GK_VirtualKey::GK_UNKNOWN,
    /* 52 */  GK_VirtualKey::GK_UNKNOWN,
    /* 53 */  GK_VirtualKey::GK_UNKNOWN,
    /* 54 */  GK_VirtualKey::GK_ShiftR,
    /* 55 */  GK_VirtualKey::GK_UNKNOWN,
    /* 56 */  GK_VirtualKey::GK_AltL,
    /* 57 */  GK_VirtualKey::GK_Space,
    /* 58 */  GK_VirtualKey::GK_CapsLock,
    /* 59 */  GK_VirtualKey::GK_F1,
    /* 60 */  GK_VirtualKey::GK_F2,
    /* 61 */  GK_VirtualKey::GK_F3,
    /* 62 */  GK_VirtualKey::GK_F4,
    /* 63 */  GK_VirtualKey::GK_F5,
    /* 64 */  GK_VirtualKey::GK_F6,
    /* 65 */  GK_VirtualKey::GK_F7,
    /* 66 */  GK_VirtualKey::GK_F8,
    /* 67 */  GK_VirtualKey::GK_F9,
    /* 68 */  GK_VirtualKey::GK_F10,
    /* 69 */  GK_VirtualKey::GK_UNKNOWN,
    /* 70 */  GK_VirtualKey::GK_UNKNOWN,
    /* 71 */  GK_VirtualKey::GK_UNKNOWN,
    /* 72 */  GK_VirtualKey::GK_UNKNOWN,
    /* 73 */  GK_VirtualKey::GK_UNKNOWN,
    /* 74 */  GK_VirtualKey::GK_UNKNOWN,
    /* 75 */  GK_VirtualKey::GK_UNKNOWN,
    /* 76 */  GK_VirtualKey::GK_UNKNOWN,
    /* 77 */  GK_VirtualKey::GK_UNKNOWN,
    /* 78 */  GK_VirtualKey::GK_UNKNOWN,
    /* 79 */  GK_VirtualKey::GK_UNKNOWN,
    /* 80 */  GK_VirtualKey::GK_UNKNOWN,
    /* 81 */  GK_VirtualKey::GK_UNKNOWN,
    /* 82 */  GK_VirtualKey::GK_UNKNOWN,
    /* 83 */  GK_VirtualKey::GK_UNKNOWN,
    /* 84 */  GK_VirtualKey::GK_UNKNOWN,
    /* 85 */  GK_VirtualKey::GK_UNKNOWN,
    /* 86 */  GK_VirtualKey::GK_UNKNOWN,
    /* 87 */  GK_VirtualKey::GK_F11,
    /* 88 */  GK_VirtualKey::GK_F12,
    /* 89 */  GK_VirtualKey::GK_UNKNOWN,
    /* 90 */  GK_VirtualKey::GK_UNKNOWN,
    /* 91 */  GK_VirtualKey::GK_UNKNOWN,
    /* 92 */  GK_VirtualKey::GK_UNKNOWN,
    /* 93 */  GK_VirtualKey::GK_UNKNOWN,
    /* 94 */  GK_VirtualKey::GK_UNKNOWN,
    /* 95 */  GK_VirtualKey::GK_UNKNOWN,
    /* 96 */  GK_VirtualKey::GK_UNKNOWN,
    /* 97 */  GK_VirtualKey::GK_ControlR,
    /* 98 */  GK_VirtualKey::GK_UNKNOWN,
    /* 99 */  GK_VirtualKey::GK_SysReq,
    /* 100 */ GK_VirtualKey::GK_AltR,
    /* 101 */ GK_VirtualKey::GK_Linefeed,
    /* 102 */ GK_VirtualKey::GK_Home,
    /* 103 */ GK_VirtualKey::GK_Up,
    /* 104 */ GK_VirtualKey::GK_PageUp,
    /* 105 */ GK_VirtualKey::GK_Left,
    /* 106 */ GK_VirtualKey::GK_Right,
    /* 107 */ GK_VirtualKey::GK_End,
    /* 108 */ GK_VirtualKey::GK_Down,
    /* 109 */ GK_VirtualKey::GK_PageDown,
    /* 110 */ GK_VirtualKey::GK_Insert,
    /* 111 */ GK_VirtualKey::GK_Delete,
    /* 112 */ GK_VirtualKey::GK_UNKNOWN,
    /* 113 */ GK_VirtualKey::GK_UNKNOWN,
    /* 114 */ GK_VirtualKey::GK_UNKNOWN,
    /* 115 */ GK_VirtualKey::GK_UNKNOWN,
    /* 116 */ GK_VirtualKey::GK_UNKNOWN,
    /* 117 */ GK_VirtualKey::GK_UNKNOWN,
    /* 118 */ GK_VirtualKey::GK_UNKNOWN,
    /* 119 */ GK_VirtualKey::GK_UNKNOWN,
    /* 120 */ GK_VirtualKey::GK_UNKNOWN,
    /* 121 */ GK_VirtualKey::GK_UNKNOWN,
    /* 122 */ GK_VirtualKey::GK_UNKNOWN,
    /* 123 */ GK_VirtualKey::GK_UNKNOWN,
    /* 124 */ GK_VirtualKey::GK_UNKNOWN,
    /* 125 */ GK_VirtualKey::GK_MetaL,
    /* 126 */ GK_VirtualKey::GK_MetaR,
    /* 127 */ GK_VirtualKey::GK_UNKNOWN,
    /* 128 */ GK_VirtualKey::GK_UNKNOWN,
    /* 129 */ GK_VirtualKey::GK_UNKNOWN,
    /* 130 */ GK_VirtualKey::GK_UNKNOWN,
    /* 131 */ GK_VirtualKey::GK_Undo,
    /* 132 */ GK_VirtualKey::GK_UNKNOWN,
    /* 133 */ GK_VirtualKey::GK_UNKNOWN,
    /* 134 */ GK_VirtualKey::GK_UNKNOWN,
    /* 135 */ GK_VirtualKey::GK_UNKNOWN,
    /* 136 */ GK_VirtualKey::GK_UNKNOWN,
    /* 137 */ GK_VirtualKey::GK_UNKNOWN,
    /* 138 */ GK_VirtualKey::GK_UNKNOWN,
    /* 139 */ GK_VirtualKey::GK_UNKNOWN,
    /* 140 */ GK_VirtualKey::GK_UNKNOWN,
    /* 141 */ GK_VirtualKey::GK_UNKNOWN,
    /* 142 */ GK_VirtualKey::GK_UNKNOWN,
    /* 143 */ GK_VirtualKey::GK_UNKNOWN,
    /* 144 */ GK_VirtualKey::GK_UNKNOWN,
    /* 145 */ GK_VirtualKey::GK_UNKNOWN,
    /* 146 */ GK_VirtualKey::GK_UNKNOWN,
    /* 147 */ GK_VirtualKey::GK_UNKNOWN,
    /* 148 */ GK_VirtualKey::GK_UNKNOWN,
    /* 149 */ GK_VirtualKey::GK_UNKNOWN,
    /* 150 */ GK_VirtualKey::GK_UNKNOWN,
    /* 151 */ GK_VirtualKey::GK_UNKNOWN,
    /* 152 */ GK_VirtualKey::GK_UNKNOWN,
    /* 153 */ GK_VirtualKey::GK_UNKNOWN,
    /* 154 */ GK_VirtualKey::GK_UNKNOWN,
    /* 155 */ GK_VirtualKey::GK_UNKNOWN,
    /* 156 */ GK_VirtualKey::GK_UNKNOWN,
    /* 157 */ GK_VirtualKey::GK_UNKNOWN,
    /* 158 */ GK_VirtualKey::GK_UNKNOWN,
    /* 159 */ GK_VirtualKey::GK_UNKNOWN,
    /* 160 */ GK_VirtualKey::GK_UNKNOWN,
    /* 161 */ GK_VirtualKey::GK_UNKNOWN,
    /* 162 */ GK_VirtualKey::GK_UNKNOWN,
    /* 163 */ GK_VirtualKey::GK_UNKNOWN,
    /* 164 */ GK_VirtualKey::GK_UNKNOWN,
    /* 165 */ GK_VirtualKey::GK_UNKNOWN,
    /* 166 */ GK_VirtualKey::GK_UNKNOWN,
    /* 167 */ GK_VirtualKey::GK_UNKNOWN,
    /* 168 */ GK_VirtualKey::GK_UNKNOWN,
    /* 169 */ GK_VirtualKey::GK_UNKNOWN,
    /* 170 */ GK_VirtualKey::GK_UNKNOWN,
    /* 171 */ GK_VirtualKey::GK_UNKNOWN,
    /* 172 */ GK_VirtualKey::GK_UNKNOWN,
    /* 173 */ GK_VirtualKey::GK_UNKNOWN,
    /* 174 */ GK_VirtualKey::GK_UNKNOWN,
    /* 175 */ GK_VirtualKey::GK_UNKNOWN,
    /* 176 */ GK_VirtualKey::GK_UNKNOWN,
    /* 177 */ GK_VirtualKey::GK_UNKNOWN,
    /* 178 */ GK_VirtualKey::GK_UNKNOWN,
    /* 179 */ GK_VirtualKey::GK_UNKNOWN,
    /* 180 */ GK_VirtualKey::GK_UNKNOWN,
    /* 181 */ GK_VirtualKey::GK_UNKNOWN,
    /* 182 */ GK_VirtualKey::GK_Redo,
    /* 183 */ GK_VirtualKey::GK_UNKNOWN,
    /* 184 */ GK_VirtualKey::GK_UNKNOWN,
    /* 185 */ GK_VirtualKey::GK_UNKNOWN,
    /* 186 */ GK_VirtualKey::GK_UNKNOWN,
    /* 187 */ GK_VirtualKey::GK_UNKNOWN,
    /* 188 */ GK_VirtualKey::GK_UNKNOWN,
    /* 189 */ GK_VirtualKey::GK_UNKNOWN,
    /* 190 */ GK_VirtualKey::GK_UNKNOWN,
    /* 191 */ GK_VirtualKey::GK_UNKNOWN,
    /* 192 */ GK_VirtualKey::GK_UNKNOWN,
    /* 193 */ GK_VirtualKey::GK_UNKNOWN,
    /* 194 */ GK_VirtualKey::GK_UNKNOWN,
    /* 195 */ GK_VirtualKey::GK_UNKNOWN,
    /* 196 */ GK_VirtualKey::GK_UNKNOWN,
    /* 197 */ GK_VirtualKey::GK_UNKNOWN,
    /* 198 */ GK_VirtualKey::GK_UNKNOWN,
    /* 199 */ GK_VirtualKey::GK_UNKNOWN,
    /* 200 */ GK_VirtualKey::GK_UNKNOWN,
    /* 201 */ GK_VirtualKey::GK_UNKNOWN,
    /* 202 */ GK_VirtualKey::GK_UNKNOWN,
    /* 203 */ GK_VirtualKey::GK_UNKNOWN,
    /* 204 */ GK_VirtualKey::GK_UNKNOWN,
    /* 205 */ GK_VirtualKey::GK_UNKNOWN,
    /* 206 */ GK_VirtualKey::GK_UNKNOWN,
    /* 207 */ GK_VirtualKey::GK_UNKNOWN,
    /* 208 */ GK_VirtualKey::GK_UNKNOWN,
    /* 209 */ GK_VirtualKey::GK_UNKNOWN,
    /* 210 */ GK_VirtualKey::GK_UNKNOWN,
    /* 211 */ GK_VirtualKey::GK_UNKNOWN,
    /* 212 */ GK_VirtualKey::GK_UNKNOWN,
    /* 213 */ GK_VirtualKey::GK_UNKNOWN,
    /* 214 */ GK_VirtualKey::GK_UNKNOWN,
    /* 215 */ GK_VirtualKey::GK_UNKNOWN,
    /* 216 */ GK_VirtualKey::GK_UNKNOWN,
    /* 217 */ GK_VirtualKey::GK_UNKNOWN,
    /* 218 */ GK_VirtualKey::GK_UNKNOWN,
    /* 219 */ GK_VirtualKey::GK_UNKNOWN,
    /* 220 */ GK_VirtualKey::GK_UNKNOWN,
    /* 221 */ GK_VirtualKey::GK_UNKNOWN,
    /* 222 */ GK_VirtualKey::GK_UNKNOWN,
    /* 223 */ GK_VirtualKey::GK_UNKNOWN,
    /* 224 */ GK_VirtualKey::GK_UNKNOWN,
    /* 225 */ GK_VirtualKey::GK_UNKNOWN,
    /* 226 */ GK_VirtualKey::GK_UNKNOWN,
    /* 227 */ GK_VirtualKey::GK_UNKNOWN,
    /* 228 */ GK_VirtualKey::GK_UNKNOWN,
    /* 229 */ GK_VirtualKey::GK_UNKNOWN,
    /* 230 */ GK_VirtualKey::GK_UNKNOWN,
    /* 231 */ GK_VirtualKey::GK_UNKNOWN,
    /* 232 */ GK_VirtualKey::GK_UNKNOWN,
    /* 233 */ GK_VirtualKey::GK_UNKNOWN,
    /* 234 */ GK_VirtualKey::GK_UNKNOWN,
    /* 235 */ GK_VirtualKey::GK_UNKNOWN,
    /* 236 */ GK_VirtualKey::GK_UNKNOWN,
    /* 237 */ GK_VirtualKey::GK_UNKNOWN,
    /* 238 */ GK_VirtualKey::GK_UNKNOWN,
    /* 239 */ GK_VirtualKey::GK_UNKNOWN,
    /* 240 */ GK_VirtualKey::GK_UNKNOWN,
    /* 241 */ GK_VirtualKey::GK_UNKNOWN,
    /* 242 */ GK_VirtualKey::GK_UNKNOWN,
    /* 243 */ GK_VirtualKey::GK_UNKNOWN,
    /* 244 */ GK_VirtualKey::GK_UNKNOWN,
    /* 245 */ GK_VirtualKey::GK_UNKNOWN,
    /* 246 */ GK_VirtualKey::GK_UNKNOWN,
    /* 247 */ GK_VirtualKey::GK_UNKNOWN,
    /* 248 */ GK_VirtualKey::GK_UNKNOWN,
    /* 249 */ GK_VirtualKey::GK_UNKNOWN,
    /* 250 */ GK_VirtualKey::GK_UNKNOWN,
    /* 251 */ GK_VirtualKey::GK_UNKNOWN,
    /* 252 */ GK_VirtualKey::GK_UNKNOWN,
    /* 253 */ GK_VirtualKey::GK_UNKNOWN,
    /* 254 */ GK_VirtualKey::GK_UNKNOWN,
    /* 255 */ GK_VirtualKey::GK_UNKNOWN
};

#endif  //_VIRTUALKEYSLINUX_H
 
