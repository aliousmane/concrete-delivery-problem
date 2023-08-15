Minimize
 obj1: fin
Subject To
 c1:   x1 - x2 >= 0
 c2:   x3 - x4 >= 0
 c3:   x5 - x6 >= 0
 c4:   x7 - x8 >= 0
 c5:   x9 - x10 >= 0
 c6:   x11 - x12 >= 0
 c7:   x13 - x14 >= 0
 c8:   x15 - x16 >= 0
 c9:   x17 - x18 >= 0
 c10:  x19 - x20 >= 0
 c11:  x21 - x22 >= 0
 c12:  DEnd_11_17 - DStart_11_17 >= 0
 c13:  DEnd_12_18 - DStart_12_18 >= 0
 c14:  DEnd_13_19 - DStart_13_19 >= 0
 c15:  DEnd_14_20 - DStart_14_20 >= 0
 c16:  DEnd_15_21 - DStart_15_21 >= 0
 c17:  DEnd_16_22 - DStart_16_22 >= 0
 c18:  LEnd_17 - LStart_17 >= 0
 c19:  LEnd_18 - LStart_18 >= 0
 c20:  LEnd_19 - LStart_19 >= 0
 c21:  LEnd_20 - LStart_20 >= 0
 c22:  LEnd_21 - LStart_21 >= 0
 c23:  LEnd_22 - LStart_22 >= 0
 c24:  - x15 + LStart_17 >= 22
 c25:  x15 - x16 >= 0
 c26:  - x16 + LStart_17 >= 0
 c27:  - x15 + fin >= 0
 c28:  DStart_11_17 - LEnd_17 >= 11
 c29:  LEnd_17 - LStart_17 >= 0
 c30:  DStart_11_17 - LStart_17 >= 0
 c31:  DStart_11_17 >= 300
 c32:  DStart_11_17 <= 400
 c33:  DEnd_11_17 <= 410
 c34:  DEnd_11_17 - DStart_11_17  = 10
 c35:  - DStart_11_17 + LEnd_17  = -11
 c36:  - LEnd_17 + fin >= 0
 c37:  - DEnd_11_17 + LStart_19 >= 11
 c38:  DEnd_11_17 - DStart_11_17 >= 0
 c39:  - DStart_11_17 + LStart_19 >= 0
 c40:  - DEnd_11_17 + fin >= 0
 c41:  DStart_13_19 - LEnd_19 >= 11
 c42:  LEnd_19 - LStart_19 >= 0
 c43:  DStart_13_19 - LStart_19 >= 0
 c44:  DStart_13_19 >= 300
 c45:  DStart_13_19 <= 400
 c46:  DEnd_13_19 <= 410
 c47:  DEnd_13_19 - DStart_13_19  = 10
 c48:  - DStart_13_19 + LEnd_19  = -11
 c49:  - LEnd_19 + fin >= 0
 c50:  - DEnd_13_19 + LStart_21 >= 11
 c51:  DEnd_13_19 - DStart_13_19 >= 0
 c52:  - DStart_13_19 + LStart_21 >= 0
 c53:  - DEnd_13_19 + fin >= 0
 c54:  DStart_15_21 - LEnd_21 >= 11
 c55:  LEnd_21 - LStart_21 >= 0
 c56:  DStart_15_21 - LStart_21 >= 0
 c57:  DStart_15_21 >= 300
 c58:  DStart_15_21 <= 400
 c59:  DEnd_15_21 <= 410
 c60:  DEnd_15_21 - DStart_15_21  = 10
 c61:  - DStart_15_21 + LEnd_21  = -11
 c62:  - LEnd_21 + fin >= 0
 c63:  x18 - DEnd_15_21 >= 12
 c64:  DEnd_15_21 - DStart_15_21 >= 0
 c65:  x18 - DStart_15_21 >= 0
 c66:  - DEnd_15_21 + fin >= 0
 c67:  - x17 + fin >= 0
 c68:  - x19 + LStart_18 >= 22
 c69:  x19 - x20 >= 0
 c70:  - x20 + LStart_18 >= 0
 c71:  - x19 + fin >= 0
 c72:  DStart_12_18 - LEnd_18 >= 11
 c73:  LEnd_18 - LStart_18 >= 0
 c74:  DStart_12_18 - LStart_18 >= 0
 c75:  DStart_12_18 >= 300
 c76:  DStart_12_18 <= 395
 c77:  DEnd_12_18 <= 410
 c78:  DEnd_12_18 - DStart_12_18  = 15
 c79:  - DStart_12_18 + LEnd_18  = -11
 c80:  - LEnd_18 + fin >= 0
 c81:  - DEnd_12_18 + LStart_20 >= 11
 c82:  DEnd_12_18 - DStart_12_18 >= 0
 c83:  - DStart_12_18 + LStart_20 >= 0
 c84:  - DEnd_12_18 + fin >= 0
 c85:  DStart_14_20 - LEnd_20 >= 11
 c86:  LEnd_20 - LStart_20 >= 0
 c87:  DStart_14_20 - LStart_20 >= 0
 c88:  DStart_14_20 >= 300
 c89:  DStart_14_20 <= 395
 c90:  DEnd_14_20 <= 410
 c91:  DEnd_14_20 - DStart_14_20  = 15
 c92:  - DStart_14_20 + LEnd_20  = -11
 c93:  - LEnd_20 + fin >= 0
 c94:  x22 - DEnd_14_20 >= 12
 c95:  DEnd_14_20 - DStart_14_20 >= 0
 c96:  x22 - DStart_14_20 >= 0
 c97:  - DEnd_14_20 + fin >= 0
 c98:  - x21 + fin >= 0
 c99:  - DEnd_11_17 + DStart_12_18 <= 5
 c100: - DEnd_11_17 + DStart_12_18 >= 0
 c101: LEnd_17 - LStart_18 <= 0
 c102: - DEnd_12_18 + DStart_13_19 <= 5
 c103: - DEnd_12_18 + DStart_13_19 >= 0
 c104: LEnd_18 - LStart_19 <= 0
 c105: - DEnd_13_19 + DStart_14_20 <= 5
 c106: - DEnd_13_19 + DStart_14_20 >= 0
 c107: LEnd_19 - LStart_20 <= 0
 c108: - DEnd_14_20 + DStart_15_21 <= 5
 c109: - DEnd_14_20 + DStart_15_21 >= 0
 c110: LEnd_20 - LStart_21 <= 0
Bounds
 0 <= x1 <= 1440
 0 <= x2 <= 1440
 0 <= x3 <= 1440
 0 <= x4 <= 1440
 0 <= x5 <= 1440
 0 <= x6 <= 1440
 0 <= x7 <= 1440
 0 <= x8 <= 1440
 0 <= x9 <= 1440
 0 <= x10 <= 1440
 0 <= x11 <= 1440
 0 <= x12 <= 1440
 0 <= x13 <= 1440
 0 <= x14 <= 1440
 0 <= x15 <= 1440
 0 <= x16 <= 1440
 0 <= x17 <= 1440
 0 <= x18 <= 1440
 0 <= x19 <= 1440
 0 <= x20 <= 1440
 0 <= x21 <= 1440
 0 <= x22 <= 1440
 0 <= DEnd_11_17 <= 1440
 0 <= DStart_11_17 <= 1440
 0 <= DEnd_12_18 <= 1440
 0 <= DStart_12_18 <= 1440
 0 <= DEnd_13_19 <= 1440
 0 <= DStart_13_19 <= 1440
 0 <= DEnd_14_20 <= 1440
 0 <= DStart_14_20 <= 1440
 0 <= DEnd_15_21 <= 1440
 0 <= DStart_15_21 <= 1440
 0 <= DEnd_16_22 <= 1440
 0 <= DStart_16_22 <= 1440
 0 <= LEnd_17 <= 1440
 0 <= LStart_17 <= 1440
 0 <= LEnd_18 <= 1440
 0 <= LStart_18 <= 1440
 0 <= LEnd_19 <= 1440
 0 <= LStart_19 <= 1440
 0 <= LEnd_20 <= 1440
 0 <= LStart_20 <= 1440
 0 <= LEnd_21 <= 1440
 0 <= LStart_21 <= 1440
 0 <= LEnd_22 <= 1440
 0 <= LStart_22 <= 1440
 0 <= fin <= 1440
End
