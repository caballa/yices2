(set-logic QF_UFIDL)
(set-info :source |
UCLID benchmark suite.  See UCLID project: http://www.cs.cmu.edu/~uclid

This benchmark was automatically translated into SMT-LIB format from
CVC format using CVC Lite
|)
(set-info :smt-lib-version 2.0)
(set-info :category "industrial")
(set-info :status unsat)
(declare-fun BmainB_46_init_const_N1_c () Int)
(declare-fun BB_p0 () Bool)
(declare-fun BmainB_46_init_const_N1_643_c () Int)
(declare-fun BmainB_46_init_const_H_zx1_697_c () Int)
(declare-fun BmainB_46_init_const_H_zx2_711_c () Int)
(declare-fun BmainB_46_init_const_H_zN1_693_c () Int)
(declare-fun BmainB_46_init_const_zx1_697_c () Int)
(declare-fun BmainB_46_iBOT () Int)
(declare-fun BmainB_46_init_const_zx2_711_c () Int)
(declare-fun BmainB_46_init_const_zN1_693_c () Int)
(declare-fun BmainB_46_f_minus_1 (Int Int) Int)
(declare-fun BmainB_46_f_minus_2 (Int Int) Int)
(declare-fun BmainB_46_f_lt_1 (Int Int) Bool)
(declare-fun BmainB_46_f_ge_1 (Int Int) Bool)
(declare-fun BmainB_46_f_ge_2 (Int Int) Bool)
(declare-fun BmainB_46_f_ge_3 (Int Int) Bool)
(assert (let ((?v_0 (+ 1 BmainB_46_iBOT))) (let ((?v_1 (+ 1 ?v_0))) (let ((?v_7 (ite BB_p0 ?v_0 ?v_1))) (let ((?v_3 (= ?v_7 ?v_1))) (let ((?v_2 (BmainB_46_f_minus_1 BmainB_46_init_const_N1_643_c (ite ?v_3 BmainB_46_init_const_N1_643_c BmainB_46_init_const_H_zN1_693_c))) (?v_12 (+ 1 ?v_1))) (let ((?v_13 (+ 1 ?v_12))) (let ((?v_14 (+ 1 ?v_13))) (let ((?v_15 (+ 1 ?v_14))) (let ((?v_16 (+ 1 ?v_15))) (let ((?v_17 (+ 1 ?v_16))) (let ((?v_18 (+ 1 ?v_17))) (let ((?v_27 (+ 1 ?v_18))) (let ((?v_5 (+ 1 ?v_27))) (let ((?v_8 (BmainB_46_f_minus_2 ?v_2 ?v_5)) (?v_4 (BmainB_46_f_minus_1 BmainB_46_init_const_N1_c (ite ?v_3 BmainB_46_init_const_N1_c BmainB_46_init_const_H_zN1_693_c))) (?v_6 (not (= BmainB_46_init_const_N1_c BmainB_46_iBOT)))) (let ((?v_57 (ite ?v_6 ?v_4 BmainB_46_iBOT))) (let ((?v_46 (BmainB_46_f_minus_2 ?v_57 ?v_5)) (?v_169 (not (= BmainB_46_init_const_H_zx1_697_c BmainB_46_iBOT))) (?v_9 (+ 1 ?v_5)) (?v_10 (ite ?v_3 ?v_0 ?v_7)) (?v_11 (= (ite ?v_3 (ite ?v_3 ?v_0 ?v_1) ?v_7) ?v_1)) (?v_87 (= BmainB_46_init_const_H_zx1_697_c ?v_0))) (let ((?v_26 (and ?v_11 ?v_87)) (?v_89 (= BmainB_46_init_const_H_zx1_697_c ?v_1))) (let ((?v_25 (or ?v_26 (and ?v_11 ?v_89))) (?v_91 (= BmainB_46_init_const_H_zx1_697_c ?v_12))) (let ((?v_24 (or ?v_25 (and ?v_11 ?v_91))) (?v_93 (= BmainB_46_init_const_H_zx1_697_c ?v_13))) (let ((?v_23 (or ?v_24 (and ?v_11 ?v_93))) (?v_95 (= BmainB_46_init_const_H_zx1_697_c ?v_14))) (let ((?v_22 (or ?v_23 (and ?v_11 ?v_95))) (?v_97 (= BmainB_46_init_const_H_zx1_697_c ?v_15))) (let ((?v_21 (or ?v_22 (and ?v_11 ?v_97))) (?v_99 (= BmainB_46_init_const_H_zx1_697_c ?v_16))) (let ((?v_20 (or ?v_21 (and ?v_11 ?v_99))) (?v_101 (= BmainB_46_init_const_H_zx1_697_c ?v_17))) (let ((?v_19 (or ?v_20 (and ?v_11 ?v_101))) (?v_103 (= BmainB_46_init_const_H_zx1_697_c ?v_18)) (?v_167 (= BmainB_46_init_const_H_zx2_711_c ?v_1))) (let ((?v_30 (and ?v_11 ?v_167)) (?v_28 (= BmainB_46_init_const_H_zx1_697_c ?v_27))) (let ((?v_29 (not ?v_28)) (?v_171 (= BmainB_46_init_const_H_zx2_711_c ?v_0))) (let ((?v_45 (or (and ?v_30 ?v_29) (and (and ?v_11 ?v_171) ?v_28))) (?v_173 (= BmainB_46_init_const_H_zx2_711_c ?v_12))) (let ((?v_31 (and ?v_11 ?v_173))) (let ((?v_44 (or ?v_45 (or (and ?v_31 ?v_29) (and ?v_30 ?v_28)))) (?v_178 (= BmainB_46_init_const_H_zx2_711_c ?v_13))) (let ((?v_32 (and ?v_11 ?v_178))) (let ((?v_43 (or ?v_44 (or (and ?v_32 ?v_29) (and ?v_31 ?v_28)))) (?v_181 (= BmainB_46_init_const_H_zx2_711_c ?v_14))) (let ((?v_33 (and ?v_11 ?v_181))) (let ((?v_42 (or ?v_43 (or (and ?v_33 ?v_29) (and ?v_32 ?v_28)))) (?v_184 (= BmainB_46_init_const_H_zx2_711_c ?v_15))) (let ((?v_34 (and ?v_11 ?v_184))) (let ((?v_41 (or ?v_42 (or (and ?v_34 ?v_29) (and ?v_33 ?v_28)))) (?v_187 (= BmainB_46_init_const_H_zx2_711_c ?v_16))) (let ((?v_35 (and ?v_11 ?v_187))) (let ((?v_40 (or ?v_41 (or (and ?v_35 ?v_29) (and ?v_34 ?v_28)))) (?v_190 (= BmainB_46_init_const_H_zx2_711_c ?v_17))) (let ((?v_36 (and ?v_11 ?v_190))) (let ((?v_39 (or ?v_40 (or (and ?v_36 ?v_29) (and ?v_35 ?v_28)))) (?v_193 (= BmainB_46_init_const_H_zx2_711_c ?v_18))) (let ((?v_37 (and ?v_11 ?v_193))) (let ((?v_38 (or ?v_39 (or (and ?v_37 ?v_29) (and ?v_36 ?v_28)))) (?v_196 (= BmainB_46_init_const_H_zx2_711_c ?v_27)) (?v_49 (not (= ?v_7 BmainB_46_iBOT)))) (let ((?v_47 (or ?v_3 ?v_49))) (let ((?v_48 (ite ?v_47 (ite (= (ite ?v_3 (ite (BmainB_46_f_lt_1 ?v_9 ?v_46) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_47 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_54 (and ?v_3 (= ?v_48 ?v_1))) (?v_55 (and ?v_3 (= ?v_48 ?v_0)))) (let ((?v_53 (or ?v_54 ?v_55)) (?v_75 (not (= ?v_1 BmainB_46_iBOT)))) (let ((?v_50 (or ?v_49 ?v_75)) (?v_83 (ite ?v_49 ?v_7 BmainB_46_iBOT))) (let ((?v_51 (ite ?v_50 (ite (= ?v_83 ?v_1) ?v_1 (ite ?v_50 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_52 (and ?v_53 (= ?v_51 ?v_1))) (?v_56 (= ?v_51 ?v_0))) (let ((?v_58 (ite (or ?v_52 ?v_56) (ite ?v_52 (ite ?v_53 (ite ?v_54 (ite ?v_3 (ite ?v_3 ?v_0 BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_55 (ite ?v_3 (ite ?v_3 ?v_1 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) (ite ?v_56 (ite (BmainB_46_f_ge_1 ?v_9 ?v_57) ?v_1 ?v_0) BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_59 (not (= ?v_58 BmainB_46_iBOT))) (?v_84 (ite ?v_49 ?v_10 BmainB_46_iBOT)) (?v_61 (= (ite ?v_49 (ite (and ?v_3 (= (ite ?v_49 (ite (= ?v_7 ?v_0) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1)) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_60 (ite ?v_49 (ite (or ?v_3 ?v_61) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_62 (ite (not (= ?v_60 BmainB_46_iBOT)) (ite (or (= ?v_60 ?v_1) ?v_61) ?v_1 ?v_0) BmainB_46_iBOT)) (?v_65 (ite ?v_49 (ite ?v_3 ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_66 (= ?v_65 ?v_1))) (let ((?v_63 (ite (not (= ?v_62 BmainB_46_iBOT)) (ite (or (= ?v_62 ?v_1) ?v_66) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_64 (ite (not (= ?v_63 BmainB_46_iBOT)) (ite (= ?v_63 ?v_1) ?v_62 ?v_7) BmainB_46_iBOT))) (let ((?v_69 (not (= ?v_64 BmainB_46_iBOT))) (?v_67 (not (= ?v_65 BmainB_46_iBOT)))) (let ((?v_68 (ite ?v_67 (ite (or ?v_66 (= (ite ?v_67 (ite (or ?v_66 ?v_61) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_70 (ite ?v_69 (ite (or (= ?v_64 ?v_1) (= (ite (not (= ?v_68 BmainB_46_iBOT)) (ite (= ?v_68 ?v_1) ?v_65 ?v_7) BmainB_46_iBOT) ?v_1)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_72 (= ?v_70 ?v_1)) (?v_74 (not (= ?v_70 BmainB_46_iBOT)))) (let ((?v_71 (or ?v_69 ?v_74))) (let ((?v_73 (ite ?v_71 (ite (= (ite ?v_69 ?v_64 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_71 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_80 (and ?v_72 (= ?v_73 ?v_1))) (?v_81 (and ?v_72 (= ?v_73 ?v_0)))) (let ((?v_79 (or ?v_80 ?v_81)) (?v_76 (or ?v_74 ?v_75))) (let ((?v_77 (ite ?v_76 (ite (= (ite ?v_74 ?v_70 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_76 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_78 (and ?v_79 (= ?v_77 ?v_1))) (?v_82 (and ?v_49 (= ?v_77 ?v_0)))) (let ((?v_86 (ite (or ?v_78 ?v_82) (ite ?v_78 (ite ?v_79 (ite ?v_80 (ite ?v_72 (ite ?v_72 ?v_0 BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_81 (ite ?v_72 (ite ?v_72 ?v_1 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) (ite ?v_82 ?v_83 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_85 (= ?v_86 ?v_1)) (?v_166 (not (= ?v_86 BmainB_46_iBOT)))) (let ((?v_88 (ite ?v_166 (ite (and ?v_85 ?v_87) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_111 (not (= ?v_88 BmainB_46_iBOT)))) (let ((?v_90 (ite ?v_111 (ite (or (= ?v_88 ?v_1) (and ?v_85 ?v_89)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_112 (not (= ?v_90 BmainB_46_iBOT))) (?v_110 (= ?v_90 ?v_1))) (let ((?v_92 (ite ?v_112 (ite (or ?v_110 (and ?v_85 ?v_91)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_115 (not (= ?v_92 BmainB_46_iBOT))) (?v_109 (= ?v_92 ?v_1))) (let ((?v_94 (ite ?v_115 (ite (or ?v_109 (and ?v_85 ?v_93)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_118 (not (= ?v_94 BmainB_46_iBOT))) (?v_108 (= ?v_94 ?v_1))) (let ((?v_96 (ite ?v_118 (ite (or ?v_108 (and ?v_85 ?v_95)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_121 (not (= ?v_96 BmainB_46_iBOT))) (?v_107 (= ?v_96 ?v_1))) (let ((?v_98 (ite ?v_121 (ite (or ?v_107 (and ?v_85 ?v_97)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_124 (not (= ?v_98 BmainB_46_iBOT))) (?v_106 (= ?v_98 ?v_1))) (let ((?v_100 (ite ?v_124 (ite (or ?v_106 (and ?v_85 ?v_99)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_127 (not (= ?v_100 BmainB_46_iBOT))) (?v_105 (= ?v_100 ?v_1))) (let ((?v_102 (ite ?v_127 (ite (or ?v_105 (and ?v_85 ?v_101)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_130 (not (= ?v_102 BmainB_46_iBOT))) (?v_104 (= ?v_102 ?v_1))) (let ((?v_133 (ite ?v_130 (ite (or ?v_104 (and ?v_85 ?v_103)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_135 (= ?v_133 ?v_1)) (?v_113 (or ?v_111 ?v_112))) (let ((?v_114 (ite ?v_113 (ite (= (ite ?v_111 ?v_88 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_113 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_156 (and ?v_110 (= ?v_114 ?v_1))) (?v_157 (and ?v_110 (= ?v_114 ?v_0)))) (let ((?v_155 (and ?v_109 (or ?v_156 ?v_157))) (?v_116 (or ?v_112 ?v_115))) (let ((?v_117 (ite ?v_116 (ite (= (ite ?v_112 ?v_90 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_116 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_154 (and ?v_155 (= ?v_117 ?v_1))) (?v_158 (and ?v_109 (= ?v_117 ?v_0)))) (let ((?v_153 (and ?v_108 (or ?v_154 ?v_158))) (?v_119 (or ?v_115 ?v_118))) (let ((?v_120 (ite ?v_119 (ite (= (ite ?v_115 ?v_92 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_119 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_152 (and ?v_153 (= ?v_120 ?v_1))) (?v_159 (and ?v_108 (= ?v_120 ?v_0)))) (let ((?v_151 (and ?v_107 (or ?v_152 ?v_159))) (?v_122 (or ?v_118 ?v_121))) (let ((?v_123 (ite ?v_122 (ite (= (ite ?v_118 ?v_94 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_122 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_150 (and ?v_151 (= ?v_123 ?v_1))) (?v_160 (and ?v_107 (= ?v_123 ?v_0)))) (let ((?v_149 (and ?v_106 (or ?v_150 ?v_160))) (?v_125 (or ?v_121 ?v_124))) (let ((?v_126 (ite ?v_125 (ite (= (ite ?v_121 ?v_96 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_125 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_148 (and ?v_149 (= ?v_126 ?v_1))) (?v_161 (and ?v_106 (= ?v_126 ?v_0)))) (let ((?v_147 (and ?v_105 (or ?v_148 ?v_161))) (?v_128 (or ?v_124 ?v_127))) (let ((?v_129 (ite ?v_128 (ite (= (ite ?v_124 ?v_98 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_128 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_146 (and ?v_147 (= ?v_129 ?v_1))) (?v_162 (and ?v_105 (= ?v_129 ?v_0)))) (let ((?v_145 (and ?v_104 (or ?v_146 ?v_162))) (?v_131 (or ?v_127 ?v_130))) (let ((?v_132 (ite ?v_131 (ite (= (ite ?v_127 ?v_100 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_131 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_144 (and ?v_145 (= ?v_132 ?v_1))) (?v_163 (and ?v_104 (= ?v_132 ?v_0)))) (let ((?v_143 (and ?v_135 (or ?v_144 ?v_163))) (?v_137 (not (= ?v_133 BmainB_46_iBOT)))) (let ((?v_134 (or ?v_130 ?v_137))) (let ((?v_136 (ite ?v_134 (ite (= (ite ?v_130 ?v_102 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_134 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_142 (and ?v_143 (= ?v_136 ?v_1))) (?v_164 (and ?v_135 (= ?v_136 ?v_0)))) (let ((?v_141 (or ?v_142 ?v_164)) (?v_138 (or ?v_137 ?v_75))) (let ((?v_139 (ite ?v_138 (ite (= (ite ?v_137 ?v_133 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_138 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_140 (and ?v_141 (= ?v_139 ?v_1))) (?v_165 (= ?v_139 ?v_0)) (?v_168 (ite ?v_166 (ite (and ?v_85 ?v_167) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_175 (= ?v_168 ?v_1)) (?v_170 (ite ?v_169 (ite ?v_28 ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_174 (= (ite (not (= ?v_170 BmainB_46_iBOT)) (ite (= ?v_170 ?v_0) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1)) (?v_176 (= ?v_170 ?v_1))) (let ((?v_172 (ite (not (= ?v_168 BmainB_46_iBOT)) (ite (or (and ?v_175 ?v_174) (and (and ?v_85 ?v_171) ?v_176)) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_205 (not (= ?v_172 BmainB_46_iBOT))) (?v_179 (= (ite ?v_166 (ite (and ?v_85 ?v_173) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_177 (ite ?v_205 (ite (or (= ?v_172 ?v_1) (or (and ?v_179 ?v_174) (and ?v_175 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_206 (not (= ?v_177 BmainB_46_iBOT))) (?v_204 (= ?v_177 ?v_1)) (?v_182 (= (ite ?v_166 (ite (and ?v_85 ?v_178) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_180 (ite ?v_206 (ite (or ?v_204 (or (and ?v_182 ?v_174) (and ?v_179 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_209 (not (= ?v_180 BmainB_46_iBOT))) (?v_203 (= ?v_180 ?v_1)) (?v_185 (= (ite ?v_166 (ite (and ?v_85 ?v_181) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_183 (ite ?v_209 (ite (or ?v_203 (or (and ?v_185 ?v_174) (and ?v_182 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_212 (not (= ?v_183 BmainB_46_iBOT))) (?v_202 (= ?v_183 ?v_1)) (?v_188 (= (ite ?v_166 (ite (and ?v_85 ?v_184) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_186 (ite ?v_212 (ite (or ?v_202 (or (and ?v_188 ?v_174) (and ?v_185 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_215 (not (= ?v_186 BmainB_46_iBOT))) (?v_201 (= ?v_186 ?v_1)) (?v_191 (= (ite ?v_166 (ite (and ?v_85 ?v_187) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_189 (ite ?v_215 (ite (or ?v_201 (or (and ?v_191 ?v_174) (and ?v_188 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_218 (not (= ?v_189 BmainB_46_iBOT))) (?v_200 (= ?v_189 ?v_1)) (?v_194 (= (ite ?v_166 (ite (and ?v_85 ?v_190) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_192 (ite ?v_218 (ite (or ?v_200 (or (and ?v_194 ?v_174) (and ?v_191 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_221 (not (= ?v_192 BmainB_46_iBOT))) (?v_199 (= ?v_192 ?v_1)) (?v_197 (= (ite ?v_166 (ite (and ?v_85 ?v_193) ?v_1 ?v_0) BmainB_46_iBOT) ?v_1))) (let ((?v_195 (ite ?v_221 (ite (or ?v_199 (or (and ?v_197 ?v_174) (and ?v_194 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_224 (not (= ?v_195 BmainB_46_iBOT))) (?v_198 (= ?v_195 ?v_1))) (let ((?v_227 (ite ?v_224 (ite (or ?v_198 (or (and (and ?v_85 ?v_196) ?v_174) (and ?v_197 ?v_176))) ?v_1 ?v_0) BmainB_46_iBOT))) (let ((?v_229 (= ?v_227 ?v_1)) (?v_207 (or ?v_205 ?v_206))) (let ((?v_208 (ite ?v_207 (ite (= (ite ?v_205 ?v_172 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_207 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_250 (and ?v_204 (= ?v_208 ?v_1))) (?v_251 (and ?v_204 (= ?v_208 ?v_0)))) (let ((?v_249 (and ?v_203 (or ?v_250 ?v_251))) (?v_210 (or ?v_206 ?v_209))) (let ((?v_211 (ite ?v_210 (ite (= (ite ?v_206 ?v_177 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_210 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_248 (and ?v_249 (= ?v_211 ?v_1))) (?v_252 (and ?v_203 (= ?v_211 ?v_0)))) (let ((?v_247 (and ?v_202 (or ?v_248 ?v_252))) (?v_213 (or ?v_209 ?v_212))) (let ((?v_214 (ite ?v_213 (ite (= (ite ?v_209 ?v_180 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_213 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_246 (and ?v_247 (= ?v_214 ?v_1))) (?v_253 (and ?v_202 (= ?v_214 ?v_0)))) (let ((?v_245 (and ?v_201 (or ?v_246 ?v_253))) (?v_216 (or ?v_212 ?v_215))) (let ((?v_217 (ite ?v_216 (ite (= (ite ?v_212 ?v_183 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_216 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_244 (and ?v_245 (= ?v_217 ?v_1))) (?v_254 (and ?v_201 (= ?v_217 ?v_0)))) (let ((?v_243 (and ?v_200 (or ?v_244 ?v_254))) (?v_219 (or ?v_215 ?v_218))) (let ((?v_220 (ite ?v_219 (ite (= (ite ?v_215 ?v_186 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_219 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_242 (and ?v_243 (= ?v_220 ?v_1))) (?v_255 (and ?v_200 (= ?v_220 ?v_0)))) (let ((?v_241 (and ?v_199 (or ?v_242 ?v_255))) (?v_222 (or ?v_218 ?v_221))) (let ((?v_223 (ite ?v_222 (ite (= (ite ?v_218 ?v_189 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_222 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_240 (and ?v_241 (= ?v_223 ?v_1))) (?v_256 (and ?v_199 (= ?v_223 ?v_0)))) (let ((?v_239 (and ?v_198 (or ?v_240 ?v_256))) (?v_225 (or ?v_221 ?v_224))) (let ((?v_226 (ite ?v_225 (ite (= (ite ?v_221 ?v_192 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_225 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_238 (and ?v_239 (= ?v_226 ?v_1))) (?v_257 (and ?v_198 (= ?v_226 ?v_0)))) (let ((?v_237 (and ?v_229 (or ?v_238 ?v_257))) (?v_231 (not (= ?v_227 BmainB_46_iBOT)))) (let ((?v_228 (or ?v_224 ?v_231))) (let ((?v_230 (ite ?v_228 (ite (= (ite ?v_224 ?v_195 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_228 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_236 (and ?v_237 (= ?v_230 ?v_1))) (?v_258 (and ?v_229 (= ?v_230 ?v_0)))) (let ((?v_235 (or ?v_236 ?v_258)) (?v_232 (or ?v_231 ?v_75))) (let ((?v_233 (ite ?v_232 (ite (= (ite ?v_231 ?v_227 BmainB_46_iBOT) ?v_1) ?v_1 (ite ?v_232 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT))) (let ((?v_234 (and ?v_235 (= ?v_233 ?v_1))) (?v_259 (= ?v_233 ?v_0))) (let ((?v_260 (ite (and (= ?v_84 ?v_1) ?v_85) ?v_84 (ite (and (and ?v_85 (BmainB_46_f_ge_2 (ite (or ?v_140 ?v_165) (ite ?v_140 (ite ?v_141 (ite ?v_142 (ite ?v_143 (ite ?v_135 (ite ?v_144 (ite ?v_145 (ite ?v_104 (ite ?v_146 (ite ?v_147 (ite ?v_105 (ite ?v_148 (ite ?v_149 (ite ?v_106 (ite ?v_150 (ite ?v_151 (ite ?v_107 (ite ?v_152 (ite ?v_153 (ite ?v_108 (ite ?v_154 (ite ?v_155 (ite ?v_109 (ite ?v_156 (ite ?v_110 (ite ?v_110 ?v_1 BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_157 (ite ?v_110 (ite ?v_110 ?v_12 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_158 (ite ?v_109 (ite ?v_109 ?v_13 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_159 (ite ?v_108 (ite ?v_108 ?v_14 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_160 (ite ?v_107 (ite ?v_107 ?v_15 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_161 (ite ?v_106 (ite ?v_106 ?v_16 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_162 (ite ?v_105 (ite ?v_105 ?v_17 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_163 (ite ?v_104 (ite ?v_104 ?v_18 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_164 (ite ?v_135 (ite ?v_135 ?v_27 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) (ite ?v_165 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT) ?v_0)) (BmainB_46_f_ge_3 (ite (or ?v_234 ?v_259) (ite ?v_234 (ite ?v_235 (ite ?v_236 (ite ?v_237 (ite ?v_229 (ite ?v_238 (ite ?v_239 (ite ?v_198 (ite ?v_240 (ite ?v_241 (ite ?v_199 (ite ?v_242 (ite ?v_243 (ite ?v_200 (ite ?v_244 (ite ?v_245 (ite ?v_201 (ite ?v_246 (ite ?v_247 (ite ?v_202 (ite ?v_248 (ite ?v_249 (ite ?v_203 (ite ?v_250 (ite ?v_204 (ite ?v_204 ?v_1 BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_251 (ite ?v_204 (ite ?v_204 ?v_12 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_252 (ite ?v_203 (ite ?v_203 ?v_13 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_253 (ite ?v_202 (ite ?v_202 ?v_14 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_254 (ite ?v_201 (ite ?v_201 ?v_15 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_255 (ite ?v_200 (ite ?v_200 ?v_16 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_256 (ite ?v_199 (ite ?v_199 ?v_17 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_257 (ite ?v_198 (ite ?v_198 ?v_18 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) BmainB_46_iBOT) (ite ?v_258 (ite ?v_229 (ite ?v_229 ?v_27 BmainB_46_iBOT) BmainB_46_iBOT) BmainB_46_iBOT)) BmainB_46_iBOT) (ite ?v_259 ?v_0 BmainB_46_iBOT)) BmainB_46_iBOT) ?v_17)) ?v_1 ?v_0)))) (let ((?v_261 (not (= ?v_260 BmainB_46_iBOT)))) (not (or (not (and (and (and (and (and (and (and (and (and (and (and (and (not (= ?v_2 BmainB_46_iBOT)) (not (= ?v_8 BmainB_46_iBOT))) (not (= ?v_4 BmainB_46_iBOT))) (not (= ?v_46 BmainB_46_iBOT))) (not (= BmainB_46_init_const_zx1_697_c BmainB_46_iBOT))) ?v_169) (not (= BmainB_46_init_const_zx2_711_c BmainB_46_iBOT))) (not (= BmainB_46_init_const_H_zx2_711_c BmainB_46_iBOT))) (not (= BmainB_46_init_const_N1_643_c BmainB_46_iBOT))) ?v_6) (not (= BmainB_46_init_const_zN1_693_c BmainB_46_iBOT))) (not (= BmainB_46_init_const_H_zN1_693_c BmainB_46_iBOT))) (and (and (and (and (and (= ?v_7 (ite ?v_3 (ite (BmainB_46_f_lt_1 ?v_9 ?v_8) ?v_0 ?v_1) (ite (BmainB_46_f_ge_1 ?v_9 ?v_2) ?v_1 ?v_0))) (= ?v_7 (ite (and (= ?v_10 ?v_1) ?v_11) ?v_10 (ite (and (and ?v_11 (BmainB_46_f_ge_2 (ite (or ?v_19 (and ?v_11 ?v_103)) (ite ?v_19 (ite ?v_20 (ite ?v_21 (ite ?v_22 (ite ?v_23 (ite ?v_24 (ite ?v_25 (ite ?v_26 ?v_1 ?v_12) ?v_13) ?v_14) ?v_15) ?v_16) ?v_17) ?v_18) ?v_27) ?v_0) ?v_0)) (BmainB_46_f_ge_3 (ite (or ?v_38 (or (and (and ?v_11 ?v_196) ?v_29) (and ?v_37 ?v_28))) (ite ?v_38 (ite ?v_39 (ite ?v_40 (ite ?v_41 (ite ?v_42 (ite ?v_43 (ite ?v_44 (ite ?v_45 ?v_1 ?v_12) ?v_13) ?v_14) ?v_15) ?v_16) ?v_17) ?v_18) ?v_27) ?v_0) ?v_17)) ?v_1 ?v_0)))) (= BmainB_46_init_const_H_zx1_697_c BmainB_46_init_const_zx1_697_c)) (= BmainB_46_init_const_H_zx2_711_c BmainB_46_init_const_zx2_711_c)) (= BmainB_46_init_const_N1_c BmainB_46_init_const_N1_643_c)) (= BmainB_46_init_const_H_zN1_693_c BmainB_46_init_const_zN1_693_c)))) (and (and (and (= ?v_7 (ite ?v_59 ?v_58 ?v_7)) (= ?v_7 (ite ?v_59 ?v_7 BmainB_46_iBOT))) (= ?v_7 (ite ?v_261 ?v_260 ?v_7))) (= ?v_7 (ite ?v_261 ?v_7 BmainB_46_iBOT)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))
(check-sat)
(exit)