;; example_distr.efyices converted to the SMT-LIB2 notation
(set-logic BV)


(set-option :ef-max-iters 100000)
(declare-fun fresh_value_addsub_fn_0__v1 () Bool)
(declare-fun fresh_value_addsub_fn_1__v1 () Bool)
(declare-fun fresh_value_addsub_fn_0__v2 () Bool)
(declare-fun fresh_value_addsub_fn_1__v2 () Bool)
(declare-fun fresh_value_addsub_fn_0__v3 () Bool)
(declare-fun fresh_value_addsub_fn_1__v3 () Bool)
(assert (forall ((addsub_fn_0_ Bool) (addsub_fn_1_ Bool) (alu_a_0_ Bool) (alu_a_10_ Bool) (alu_a_11_ Bool) (alu_a_12_ Bool) (alu_a_13_ Bool) (alu_a_14_ Bool) (alu_a_15_ Bool) (alu_a_16_ Bool) (alu_a_17_ Bool) (alu_a_18_ Bool) (alu_a_19_ Bool) (alu_a_1_ Bool) (alu_a_20_ Bool) (alu_a_21_ Bool) (alu_a_22_ Bool) (alu_a_23_ Bool) (alu_a_24_ Bool) (alu_a_25_ Bool) (alu_a_26_ Bool) (alu_a_27_ Bool) (alu_a_28_ Bool) (alu_a_29_ Bool) (alu_a_2_ Bool) (alu_a_30_ Bool) (alu_a_31_ Bool) (alu_a_3_ Bool) (alu_a_4_ Bool) (alu_a_5_ Bool) (alu_a_6_ Bool) (alu_a_7_ Bool) (alu_a_8_ Bool) (alu_a_9_ Bool) (alu_b_0_ Bool) (alu_b_10_ Bool) (alu_b_11_ Bool) (alu_b_12_ Bool) (alu_b_13_ Bool) (alu_b_14_ Bool) (alu_b_15_ Bool) (alu_b_16_ Bool) (alu_b_17_ Bool) (alu_b_18_ Bool) (alu_b_19_ Bool) (alu_b_1_ Bool) (alu_b_20_ Bool) (alu_b_21_ Bool) (alu_b_22_ Bool) (alu_b_23_ Bool) (alu_b_24_ Bool) (alu_b_25_ Bool) (alu_b_26_ Bool) (alu_b_27_ Bool) (alu_b_28_ Bool) (alu_b_29_ Bool) (alu_b_2_ Bool) (alu_b_30_ Bool) (alu_b_31_ Bool) (alu_b_3_ Bool) (alu_b_4_ Bool) (alu_b_5_ Bool) (alu_b_6_ Bool) (alu_b_7_ Bool) (alu_b_8_ Bool) (alu_b_9_ Bool) (result_0_ Bool) (result_10_ Bool) (result_11_ Bool) (result_12_ Bool) (result_13_ Bool) (result_14_ Bool) (result_15_ Bool) (result_16_ Bool) (result_17_ Bool) (result_18_ Bool) (result_19_ Bool) (result_1_ Bool) (result_20_ Bool) (result_21_ Bool) (result_22_ Bool) (result_23_ Bool) (result_24_ Bool) (result_25_ Bool) (result_26_ Bool) (result_27_ Bool) (result_28_ Bool) (result_29_ Bool) (result_2_ Bool) (result_30_ Bool) (result_31_ Bool) (result_3_ Bool) (result_4_ Bool) (result_5_ Bool) (result_6_ Bool) (result_7_ Bool) (result_8_ Bool) (result_9_ Bool))
(and


(=>
(let ((n471 ( not ( or addsub_fn_0_ addsub_fn_1_ ) )))
	(let ((n75 ( not alu_b_8_ )))
	(let ((n77 ( not alu_b_6_ )))
	(let ((n78 ( not alu_b_5_ )))
	(let ((n80 ( not alu_b_3_ )))
	(let ((n81 ( not alu_b_2_ )))
	(let ((n82 ( not alu_b_1_ )))
	(let ((n83 ( not alu_b_0_ )))
	(let ((add_10_n1 ( and n82 n83 )))
	(let ((add_10_n2 ( and n81 add_10_n1 )))
	(let ((add_10_n3 ( and n80 add_10_n2 )))
	(let ((n79 ( not alu_b_4_ )))
	(let ((add_10_n4 ( and n79 add_10_n3 )))
	(let ((add_10_n5 ( and n78 add_10_n4 )))
	(let ((add_10_n6 ( and n77 add_10_n5 )))
	(let ((n76 ( not alu_b_7_ )))
	(let ((add_10_n7 ( and n76 add_10_n6 )))
	(let ((n470 ( xor n75 add_10_n7 )))
	(let ((xb_8_ ( or ( and n471 alu_b_8_ ) ( and ( not n471 ) n470 ) )))
	(let ((n45 ( xor n77 add_10_n5 )))
	(let ((xb_6_ ( or ( and n471 alu_b_6_ ) ( and ( not n471 ) n45 ) )))
	(let ((n41 ( xor n81 add_10_n1 )))
	(let ((n39 ( not n83 )))
	(let ((xb_0_ ( or ( and n471 alu_b_0_ ) ( and ( not n471 ) n39 ) )))
	(let ((add_11_n1 ( and xb_0_ alu_a_0_ )))
	(let ((n40 ( xor n82 n83 )))
	(let ((xb_1_ ( or ( and n471 alu_b_1_ ) ( and ( not n471 ) n40 ) )))
	(let ((add_11_carry_2_ ( or ( and ( xor alu_a_1_ xb_1_ ) add_11_n1 ) ( and alu_a_1_ xb_1_ ) )))
	(let ((xb_2_ ( or ( and n471 alu_b_2_ ) ( and ( not n471 ) n41 ) )))
	(let ((add_11_carry_3_ ( or ( and ( xor alu_a_2_ xb_2_ ) add_11_carry_2_ ) ( and alu_a_2_ xb_2_ ) )))
	(let ((n42 ( xor n80 add_10_n2 )))
	(let ((xb_3_ ( or ( and n471 alu_b_3_ ) ( and ( not n471 ) n42 ) )))
	(let ((add_11_carry_4_ ( or ( and ( xor alu_a_3_ xb_3_ ) add_11_carry_3_ ) ( and alu_a_3_ xb_3_ ) )))
	(let ((n43 ( xor n79 add_10_n3 )))
	(let ((xb_4_ ( or ( and n471 alu_b_4_ ) ( and ( not n471 ) n43 ) )))
	(let ((sum_4_ ( xor alu_a_4_ ( xor xb_4_ add_11_carry_4_ ) )))
	(let ((n651 ( not alu_b_18_ )))
	(let ((n661 ( not alu_b_17_ )))
	(let ((n671 ( not alu_b_16_ )))
	(let ((n681 ( not alu_b_15_ )))
	(let ((n691 ( not alu_b_14_ )))
	(let ((n701 ( not alu_b_13_ )))
	(let ((n71 ( not alu_b_12_ )))
	(let ((n72 ( not alu_b_11_ )))
	(let ((n73 ( not alu_b_10_ )))
	(let ((n74 ( not alu_b_9_ )))
	(let ((add_10_n8 ( and n75 add_10_n7 )))
	(let ((add_10_n9 ( and n74 add_10_n8 )))
	(let ((add_10_n10 ( and n73 add_10_n9 )))
	(let ((add_10_n11 ( and n72 add_10_n10 )))
	(let ((add_10_n12 ( and n71 add_10_n11 )))
	(let ((add_10_n13 ( and n701 add_10_n12 )))
	(let ((add_10_n14 ( and n691 add_10_n13 )))
	(let ((add_10_n15 ( and n681 add_10_n14 )))
	(let ((add_10_n16 ( and n671 add_10_n15 )))
	(let ((add_10_n17 ( and n661 add_10_n16 )))
	(let ((add_10_n18 ( and n651 add_10_n17 )))
	(let ((n641 ( not alu_b_19_ )))
	(let ((add_10_n19 ( and n641 add_10_n18 )))
	(let ((n490 ( xor n73 add_10_n9 )))
	(let ((xb_10_ ( or ( and n471 alu_b_10_ ) ( and ( not n471 ) n490 ) )))
	(let ((n46 ( xor n76 add_10_n6 )))
	(let ((xb_7_ ( or ( and n471 alu_b_7_ ) ( and ( not n471 ) n46 ) )))
	(let ((n44 ( xor n78 add_10_n4 )))
	(let ((xb_5_ ( or ( and n471 alu_b_5_ ) ( and ( not n471 ) n44 ) )))
	(let ((add_11_carry_5_ ( or ( and ( xor alu_a_4_ xb_4_ ) add_11_carry_4_ ) ( and alu_a_4_ xb_4_ ) )))
	(let ((add_11_carry_6_ ( or ( and ( xor alu_a_5_ xb_5_ ) add_11_carry_5_ ) ( and alu_a_5_ xb_5_ ) )))
	(let ((add_11_carry_7_ ( or ( and ( xor alu_a_6_ xb_6_ ) add_11_carry_6_ ) ( and alu_a_6_ xb_6_ ) )))
	(let ((add_11_carry_8_ ( or ( and ( xor alu_a_7_ xb_7_ ) add_11_carry_7_ ) ( and alu_a_7_ xb_7_ ) )))
	(let ((add_11_carry_9_ ( or ( and ( xor alu_a_8_ xb_8_ ) add_11_carry_8_ ) ( and alu_a_8_ xb_8_ ) )))
	(let ((n480 ( xor n74 add_10_n8 )))
	(let ((xb_9_ ( or ( and n471 alu_b_9_ ) ( and ( not n471 ) n480 ) )))
	(let ((add_11_carry_10_ ( or ( and ( xor alu_a_9_ xb_9_ ) add_11_carry_9_ ) ( and alu_a_9_ xb_9_ ) )))
	(let ((add_11_carry_11_ ( or ( and ( xor alu_a_10_ xb_10_ ) add_11_carry_10_ ) ( and alu_a_10_ xb_10_ ) )))
	(let ((n520 ( xor n701 add_10_n12 )))
	(let ((sum_6_ ( xor alu_a_6_ ( xor xb_6_ add_11_carry_6_ ) )))
	(let ((n530 ( xor n691 add_10_n13 )))
	(let ((n531 ( not alu_b_30_ )))
	(let ((n631 ( not alu_b_20_ )))
	(let ((n591 ( not alu_b_24_ )))
	(let ((n601 ( not alu_b_23_ )))
	(let ((n611 ( not alu_b_22_ )))
	(let ((add_10_n20 ( and n631 add_10_n19 )))
	(let ((n621 ( not alu_b_21_ )))
	(let ((add_10_n21 ( and n621 add_10_n20 )))
	(let ((add_10_n22 ( and n611 add_10_n21 )))
	(let ((add_10_n23 ( and n601 add_10_n22 )))
	(let ((n630 ( xor n591 add_10_n23 )))
	(let ((n550 ( xor n671 add_10_n15 )))
	(let ((xb_16_ ( or ( and n471 alu_b_16_ ) ( and ( not n471 ) n550 ) )))
	(let ((xb_14_ ( or ( and n471 alu_b_14_ ) ( and ( not n471 ) n530 ) )))
	(let ((xb_13_ ( or ( and n471 alu_b_13_ ) ( and ( not n471 ) n520 ) )))
	(let ((n510 ( xor n71 add_10_n11 )))
	(let ((xb_12_ ( or ( and n471 alu_b_12_ ) ( and ( not n471 ) n510 ) )))
	(let ((n500 ( xor n72 add_10_n10 )))
	(let ((xb_11_ ( or ( and n471 alu_b_11_ ) ( and ( not n471 ) n500 ) )))
	(let ((add_11_carry_12_ ( or ( and ( xor alu_a_11_ xb_11_ ) add_11_carry_11_ ) ( and alu_a_11_ xb_11_ ) )))
	(let ((add_11_carry_13_ ( or ( and ( xor alu_a_12_ xb_12_ ) add_11_carry_12_ ) ( and alu_a_12_ xb_12_ ) )))
	(let ((add_11_carry_14_ ( or ( and ( xor alu_a_13_ xb_13_ ) add_11_carry_13_ ) ( and alu_a_13_ xb_13_ ) )))
	(let ((add_11_carry_15_ ( or ( and ( xor alu_a_14_ xb_14_ ) add_11_carry_14_ ) ( and alu_a_14_ xb_14_ ) )))
	(let ((n540 ( xor n681 add_10_n14 )))
	(let ((xb_15_ ( or ( and n471 alu_b_15_ ) ( and ( not n471 ) n540 ) )))
	(let ((add_11_carry_16_ ( or ( and ( xor alu_a_15_ xb_15_ ) add_11_carry_15_ ) ( and alu_a_15_ xb_15_ ) )))
	(let ((sum_16_ ( xor alu_a_16_ ( xor xb_16_ add_11_carry_16_ ) )))
	(let ((sum_11_ ( xor alu_a_11_ ( xor xb_11_ add_11_carry_11_ ) )))
	(let ((n620 ( xor n601 add_10_n22 )))
	(let ((xb_23_ ( or ( and n471 alu_b_23_ ) ( and ( not n471 ) n620 ) )))
	(let ((n610 ( xor n611 add_10_n21 )))
	(let ((xb_22_ ( or ( and n471 alu_b_22_ ) ( and ( not n471 ) n610 ) )))
	(let ((n600 ( xor n621 add_10_n20 )))
	(let ((xb_21_ ( or ( and n471 alu_b_21_ ) ( and ( not n471 ) n600 ) )))
	(let ((n590 ( xor n631 add_10_n19 )))
	(let ((xb_20_ ( or ( and n471 alu_b_20_ ) ( and ( not n471 ) n590 ) )))
	(let ((add_11_carry_17_ ( or ( and ( xor alu_a_16_ xb_16_ ) add_11_carry_16_ ) ( and alu_a_16_ xb_16_ ) )))
	(let ((n560 ( xor n661 add_10_n16 )))
	(let ((xb_17_ ( or ( and n471 alu_b_17_ ) ( and ( not n471 ) n560 ) )))
	(let ((add_11_carry_18_ ( or ( and ( xor alu_a_17_ xb_17_ ) add_11_carry_17_ ) ( and alu_a_17_ xb_17_ ) )))
	(let ((n570 ( xor n651 add_10_n17 )))
	(let ((xb_18_ ( or ( and n471 alu_b_18_ ) ( and ( not n471 ) n570 ) )))
	(let ((add_11_carry_19_ ( or ( and ( xor alu_a_18_ xb_18_ ) add_11_carry_18_ ) ( and alu_a_18_ xb_18_ ) )))
	(let ((n580 ( xor n641 add_10_n18 )))
	(let ((xb_19_ ( or ( and n471 alu_b_19_ ) ( and ( not n471 ) n580 ) )))
	(let ((add_11_carry_20_ ( or ( and ( xor alu_a_19_ xb_19_ ) add_11_carry_19_ ) ( and alu_a_19_ xb_19_ ) )))
	(let ((add_11_carry_21_ ( or ( and ( xor alu_a_20_ xb_20_ ) add_11_carry_20_ ) ( and alu_a_20_ xb_20_ ) )))
	(let ((add_11_carry_22_ ( or ( and ( xor alu_a_21_ xb_21_ ) add_11_carry_21_ ) ( and alu_a_21_ xb_21_ ) )))
	(let ((add_11_carry_23_ ( or ( and ( xor alu_a_22_ xb_22_ ) add_11_carry_22_ ) ( and alu_a_22_ xb_22_ ) )))
	(let ((add_11_carry_24_ ( or ( and ( xor alu_a_23_ xb_23_ ) add_11_carry_23_ ) ( and alu_a_23_ xb_23_ ) )))
	(let ((n541 ( not alu_b_29_ )))
	(let ((n551 ( not alu_b_28_ )))
	(let ((n561 ( not alu_b_27_ )))
	(let ((n571 ( not alu_b_26_ )))
	(let ((add_10_n24 ( and n591 add_10_n23 )))
	(let ((n581 ( not alu_b_25_ )))
	(let ((add_10_n25 ( and n581 add_10_n24 )))
	(let ((add_10_n26 ( and n571 add_10_n25 )))
	(let ((add_10_n27 ( and n561 add_10_n26 )))
	(let ((add_10_n28 ( and n551 add_10_n27 )))
	(let ((n680 ( xor n541 add_10_n28 )))
	(let ((xb_29_ ( or ( and n471 alu_b_29_ ) ( and ( not n471 ) n680 ) )))
	(let ((n660 ( xor n561 add_10_n26 )))
	(let ((xb_27_ ( or ( and n471 alu_b_27_ ) ( and ( not n471 ) n660 ) )))
	(let ((n650 ( xor n571 add_10_n25 )))
	(let ((xb_26_ ( or ( and n471 alu_b_26_ ) ( and ( not n471 ) n650 ) )))
	(let ((xb_24_ ( or ( and n471 alu_b_24_ ) ( and ( not n471 ) n630 ) )))
	(let ((add_11_carry_25_ ( or ( and ( xor alu_a_24_ xb_24_ ) add_11_carry_24_ ) ( and alu_a_24_ xb_24_ ) )))
	(let ((n640 ( xor n581 add_10_n24 )))
	(let ((xb_25_ ( or ( and n471 alu_b_25_ ) ( and ( not n471 ) n640 ) )))
	(let ((add_11_carry_26_ ( or ( and ( xor alu_a_25_ xb_25_ ) add_11_carry_25_ ) ( and alu_a_25_ xb_25_ ) )))
	(let ((add_11_carry_27_ ( or ( and ( xor alu_a_26_ xb_26_ ) add_11_carry_26_ ) ( and alu_a_26_ xb_26_ ) )))
	(let ((add_11_carry_28_ ( or ( and ( xor alu_a_27_ xb_27_ ) add_11_carry_27_ ) ( and alu_a_27_ xb_27_ ) )))
	(let ((n670 ( xor n551 add_10_n27 )))
	(let ((xb_28_ ( or ( and n471 alu_b_28_ ) ( and ( not n471 ) n670 ) )))
	(let ((add_11_carry_29_ ( or ( and ( xor alu_a_28_ xb_28_ ) add_11_carry_28_ ) ( and alu_a_28_ xb_28_ ) )))
	(let ((sum_29_ ( xor alu_a_29_ ( xor xb_29_ add_11_carry_29_ ) )))
	(let ((sum_24_ ( xor alu_a_24_ ( xor xb_24_ add_11_carry_24_ ) )))
	(let ((sum_13_ ( xor alu_a_13_ ( xor xb_13_ add_11_carry_13_ ) )))
	(let ((sum_1_ ( xor alu_a_1_ ( xor xb_1_ add_11_n1 ) )))
	(let ((sum_28_ ( xor alu_a_28_ ( xor xb_28_ add_11_carry_28_ ) )))
	(let ((sum_3_ ( xor alu_a_3_ ( xor xb_3_ add_11_carry_3_ ) )))
	(let ((sum_26_ ( xor alu_a_26_ ( xor xb_26_ add_11_carry_26_ ) )))
	(let ((n521 ( not alu_b_31_ )))
	(let ((sum_8_ ( xor alu_a_8_ ( xor xb_8_ add_11_carry_8_ ) )))
	(let ((sum_20_ ( xor alu_a_20_ ( xor xb_20_ add_11_carry_20_ ) )))
	(let ((add_10_n29 ( and n541 add_10_n28 )))
	(let ((n690 ( xor n531 add_10_n29 )))
	(let ((xb_30_ ( or ( and n471 alu_b_30_ ) ( and ( not n471 ) n690 ) )))
	(let ((add_11_carry_30_ ( or ( and ( xor alu_a_29_ xb_29_ ) add_11_carry_29_ ) ( and alu_a_29_ xb_29_ ) )))
	(let ((add_11_carry_31_ ( or ( and ( xor alu_a_30_ xb_30_ ) add_11_carry_30_ ) ( and alu_a_30_ xb_30_ ) )))
	(let ((sum_17_ ( xor alu_a_17_ ( xor xb_17_ add_11_carry_17_ ) )))
	(let ((sum_27_ ( xor alu_a_27_ ( xor xb_27_ add_11_carry_27_ ) )))
	(let ((add_10_n30 ( and n531 add_10_n29 )))
	(let ((n700 ( xor n521 add_10_n30 )))
	(let ((xb_31_ ( or ( and n471 alu_b_31_ ) ( and ( not n471 ) n700 ) )))
	(let ((sum_31_ ( xor alu_a_31_ ( xor xb_31_ add_11_carry_31_ ) )))
	(let ((n481 ( not sum_31_ )))
	(let ((sum_9_ ( xor alu_a_9_ ( xor xb_9_ add_11_carry_9_ ) )))
	(let ((sum_22_ ( xor alu_a_22_ ( xor xb_22_ add_11_carry_22_ ) )))
	(let ((sum_2_ ( xor alu_a_2_ ( xor xb_2_ add_11_carry_2_ ) )))
	(let ((sum_18_ ( xor alu_a_18_ ( xor xb_18_ add_11_carry_18_ ) )))
	(let ((dummy0 ( or ( and ( xor alu_a_31_ xb_31_ ) add_11_carry_31_ ) ( and alu_a_31_ xb_31_ ) )))
	(let ((sum_0_ ( xor xb_0_ alu_a_0_ )))
	(let ((n511 ( not ( xor n521 alu_a_31_ ) )))
	(let ((n501 ( not ( xor addsub_fn_0_ alu_a_31_ ) )))
	(let ((n491 ( not ( or ( and n511 n501 ) ( and ( not n511 ) n481 ) ) )))
	(let ((sum_10_ ( xor alu_a_10_ ( xor xb_10_ add_11_carry_10_ ) )))
	(let ((sum_19_ ( xor alu_a_19_ ( xor xb_19_ add_11_carry_19_ ) )))
	(let ((sum_21_ ( xor alu_a_21_ ( xor xb_21_ add_11_carry_21_ ) )))
	(let ((sum_12_ ( xor alu_a_12_ ( xor xb_12_ add_11_carry_12_ ) )))
	(let ((sum_5_ ( xor alu_a_5_ ( xor xb_5_ add_11_carry_5_ ) )))
	(let ((sum_7_ ( xor alu_a_7_ ( xor xb_7_ add_11_carry_7_ ) )))
	(let ((sum_15_ ( xor alu_a_15_ ( xor xb_15_ add_11_carry_15_ ) )))
	(let ((sum_30_ ( xor alu_a_30_ ( xor xb_30_ add_11_carry_30_ ) )))
	(let ((sum_14_ ( xor alu_a_14_ ( xor xb_14_ add_11_carry_14_ ) )))
	(let ((sum_23_ ( xor alu_a_23_ ( xor xb_23_ add_11_carry_23_ ) )))
	(let ((sum_25_ ( xor alu_a_25_ ( xor xb_25_ add_11_carry_25_ ) )))
	(let ((n4 true))
	(let ((n5 false)) (and (= result_31_ ( not ( or addsub_fn_1_ n481 ) ))
(= result_30_ ( not ( or ( not sum_30_ ) addsub_fn_1_ ) ))
(= result_29_ ( not ( or ( not sum_29_ ) addsub_fn_1_ ) ))
(= result_28_ ( not ( or ( not sum_28_ ) addsub_fn_1_ ) ))
(= result_27_ ( not ( or ( not sum_27_ ) addsub_fn_1_ ) ))
(= result_26_ ( not ( or ( not sum_26_ ) addsub_fn_1_ ) ))
(= result_25_ ( not ( or ( not sum_25_ ) addsub_fn_1_ ) ))
(= result_24_ ( not ( or ( not sum_24_ ) addsub_fn_1_ ) ))
(= result_23_ ( not ( or ( not sum_23_ ) addsub_fn_1_ ) ))
(= result_22_ ( not ( or ( not sum_22_ ) addsub_fn_1_ ) ))
(= result_21_ ( not ( or ( not sum_21_ ) addsub_fn_1_ ) ))
(= result_20_ ( not ( or ( not sum_20_ ) addsub_fn_1_ ) ))
(= result_19_ ( not ( or ( not sum_19_ ) addsub_fn_1_ ) ))
(= result_18_ ( not ( or ( not sum_18_ ) addsub_fn_1_ ) ))
(= result_17_ ( not ( or ( not sum_17_ ) addsub_fn_1_ ) ))
(= result_16_ ( not ( or ( not sum_16_ ) addsub_fn_1_ ) ))
(= result_15_ ( not ( or ( not sum_15_ ) addsub_fn_1_ ) ))
(= result_14_ ( not ( or ( not sum_14_ ) addsub_fn_1_ ) ))
(= result_13_ ( not ( or ( not sum_13_ ) addsub_fn_1_ ) ))
(= result_12_ ( not ( or ( not sum_12_ ) addsub_fn_1_ ) ))
(= result_11_ ( not ( or ( not sum_11_ ) addsub_fn_1_ ) ))
(= result_10_ ( not ( or ( not sum_10_ ) addsub_fn_1_ ) ))
(= result_9_ ( not ( or ( not sum_9_ ) addsub_fn_1_ ) ))
(= result_8_ ( not ( or ( not sum_8_ ) addsub_fn_1_ ) ))
(= result_7_ ( not ( or ( not sum_7_ ) addsub_fn_1_ ) ))
(= result_6_ ( not ( or ( not sum_6_ ) addsub_fn_1_ ) ))
(= result_5_ ( not ( or ( not sum_5_ ) addsub_fn_1_ ) ))
(= result_4_ ( not ( or ( not sum_4_ ) addsub_fn_1_ ) ))
(= result_3_ ( not ( or ( not sum_3_ ) addsub_fn_1_ ) ))
(= result_2_ ( not ( or ( not sum_2_ ) addsub_fn_1_ ) ))
(= result_1_ ( not ( or ( not sum_1_ ) addsub_fn_1_ ) ))
(= result_0_ ( or ( and addsub_fn_1_ n491 ) ( and ( not addsub_fn_1_ ) sum_0_ ) ))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))
(and
(=>
    ( and
      ( = addsub_fn_0_ fresh_value_addsub_fn_0__v1 )
      ( = addsub_fn_1_ fresh_value_addsub_fn_1__v1 )
    )
    ( =
      (concat (ite result_31_ #b1 #b0) (ite result_30_ #b1 #b0) (ite result_29_ #b1 #b0) (ite result_28_ #b1 #b0) (ite result_27_ #b1 #b0) (ite result_26_ #b1 #b0) (ite result_25_ #b1 #b0) (ite result_24_ #b1 #b0) (ite result_23_ #b1 #b0) (ite result_22_ #b1 #b0) (ite result_21_ #b1 #b0) (ite result_20_ #b1 #b0) (ite result_19_ #b1 #b0) (ite result_18_ #b1 #b0) (ite result_17_ #b1 #b0) (ite result_16_ #b1 #b0) (ite result_15_ #b1 #b0) (ite result_14_ #b1 #b0) (ite result_13_ #b1 #b0) (ite result_12_ #b1 #b0) (ite result_11_ #b1 #b0) (ite result_10_ #b1 #b0) (ite result_9_ #b1 #b0) (ite result_8_ #b1 #b0) (ite result_7_ #b1 #b0) (ite result_6_ #b1 #b0) (ite result_5_ #b1 #b0) (ite result_4_ #b1 #b0) (ite result_3_ #b1 #b0) (ite result_2_ #b1 #b0) (ite result_1_ #b1 #b0) (ite result_0_ #b1 #b0))
      (bvadd
              (concat (ite alu_a_31_ #b1 #b0) (ite alu_a_30_ #b1 #b0) (ite alu_a_29_ #b1 #b0) (ite alu_a_28_ #b1 #b0) (ite alu_a_27_ #b1 #b0) (ite alu_a_26_ #b1 #b0) (ite alu_a_25_ #b1 #b0) (ite alu_a_24_ #b1 #b0) (ite alu_a_23_ #b1 #b0) (ite alu_a_22_ #b1 #b0) (ite alu_a_21_ #b1 #b0) (ite alu_a_20_ #b1 #b0) (ite alu_a_19_ #b1 #b0) (ite alu_a_18_ #b1 #b0) (ite alu_a_17_ #b1 #b0) (ite alu_a_16_ #b1 #b0) (ite alu_a_15_ #b1 #b0) (ite alu_a_14_ #b1 #b0) (ite alu_a_13_ #b1 #b0) (ite alu_a_12_ #b1 #b0) (ite alu_a_11_ #b1 #b0) (ite alu_a_10_ #b1 #b0) (ite alu_a_9_ #b1 #b0) (ite alu_a_8_ #b1 #b0) (ite alu_a_7_ #b1 #b0) (ite alu_a_6_ #b1 #b0) (ite alu_a_5_ #b1 #b0) (ite alu_a_4_ #b1 #b0) (ite alu_a_3_ #b1 #b0) (ite alu_a_2_ #b1 #b0) (ite alu_a_1_ #b1 #b0) (ite alu_a_0_ #b1 #b0))
              (concat (ite alu_b_31_ #b1 #b0) (ite alu_b_30_ #b1 #b0) (ite alu_b_29_ #b1 #b0) (ite alu_b_28_ #b1 #b0) (ite alu_b_27_ #b1 #b0) (ite alu_b_26_ #b1 #b0) (ite alu_b_25_ #b1 #b0) (ite alu_b_24_ #b1 #b0) (ite alu_b_23_ #b1 #b0) (ite alu_b_22_ #b1 #b0) (ite alu_b_21_ #b1 #b0) (ite alu_b_20_ #b1 #b0) (ite alu_b_19_ #b1 #b0) (ite alu_b_18_ #b1 #b0) (ite alu_b_17_ #b1 #b0) (ite alu_b_16_ #b1 #b0) (ite alu_b_15_ #b1 #b0) (ite alu_b_14_ #b1 #b0) (ite alu_b_13_ #b1 #b0) (ite alu_b_12_ #b1 #b0) (ite alu_b_11_ #b1 #b0) (ite alu_b_10_ #b1 #b0) (ite alu_b_9_ #b1 #b0) (ite alu_b_8_ #b1 #b0) (ite alu_b_7_ #b1 #b0) (ite alu_b_6_ #b1 #b0) (ite alu_b_5_ #b1 #b0) (ite alu_b_4_ #b1 #b0) (ite alu_b_3_ #b1 #b0) (ite alu_b_2_ #b1 #b0) (ite alu_b_1_ #b1 #b0) (ite alu_b_0_ #b1 #b0))
      )
   )
)
(=>
    ( and
      ( = addsub_fn_0_ fresh_value_addsub_fn_0__v2 )
      ( = addsub_fn_1_ fresh_value_addsub_fn_1__v2 )
    )
    ( =
      (concat (ite result_31_ #b1 #b0) (ite result_30_ #b1 #b0) (ite result_29_ #b1 #b0) (ite result_28_ #b1 #b0) (ite result_27_ #b1 #b0) (ite result_26_ #b1 #b0) (ite result_25_ #b1 #b0) (ite result_24_ #b1 #b0) (ite result_23_ #b1 #b0) (ite result_22_ #b1 #b0) (ite result_21_ #b1 #b0) (ite result_20_ #b1 #b0) (ite result_19_ #b1 #b0) (ite result_18_ #b1 #b0) (ite result_17_ #b1 #b0) (ite result_16_ #b1 #b0) (ite result_15_ #b1 #b0) (ite result_14_ #b1 #b0) (ite result_13_ #b1 #b0) (ite result_12_ #b1 #b0) (ite result_11_ #b1 #b0) (ite result_10_ #b1 #b0) (ite result_9_ #b1 #b0) (ite result_8_ #b1 #b0) (ite result_7_ #b1 #b0) (ite result_6_ #b1 #b0) (ite result_5_ #b1 #b0) (ite result_4_ #b1 #b0) (ite result_3_ #b1 #b0) (ite result_2_ #b1 #b0) (ite result_1_ #b1 #b0) (ite result_0_ #b1 #b0))
      (bvsub
              (concat (ite alu_a_31_ #b1 #b0) (ite alu_a_30_ #b1 #b0) (ite alu_a_29_ #b1 #b0) (ite alu_a_28_ #b1 #b0) (ite alu_a_27_ #b1 #b0) (ite alu_a_26_ #b1 #b0) (ite alu_a_25_ #b1 #b0) (ite alu_a_24_ #b1 #b0) (ite alu_a_23_ #b1 #b0) (ite alu_a_22_ #b1 #b0) (ite alu_a_21_ #b1 #b0) (ite alu_a_20_ #b1 #b0) (ite alu_a_19_ #b1 #b0) (ite alu_a_18_ #b1 #b0) (ite alu_a_17_ #b1 #b0) (ite alu_a_16_ #b1 #b0) (ite alu_a_15_ #b1 #b0) (ite alu_a_14_ #b1 #b0) (ite alu_a_13_ #b1 #b0) (ite alu_a_12_ #b1 #b0) (ite alu_a_11_ #b1 #b0) (ite alu_a_10_ #b1 #b0) (ite alu_a_9_ #b1 #b0) (ite alu_a_8_ #b1 #b0) (ite alu_a_7_ #b1 #b0) (ite alu_a_6_ #b1 #b0) (ite alu_a_5_ #b1 #b0) (ite alu_a_4_ #b1 #b0) (ite alu_a_3_ #b1 #b0) (ite alu_a_2_ #b1 #b0) (ite alu_a_1_ #b1 #b0) (ite alu_a_0_ #b1 #b0))
              (concat (ite alu_b_31_ #b1 #b0) (ite alu_b_30_ #b1 #b0) (ite alu_b_29_ #b1 #b0) (ite alu_b_28_ #b1 #b0) (ite alu_b_27_ #b1 #b0) (ite alu_b_26_ #b1 #b0) (ite alu_b_25_ #b1 #b0) (ite alu_b_24_ #b1 #b0) (ite alu_b_23_ #b1 #b0) (ite alu_b_22_ #b1 #b0) (ite alu_b_21_ #b1 #b0) (ite alu_b_20_ #b1 #b0) (ite alu_b_19_ #b1 #b0) (ite alu_b_18_ #b1 #b0) (ite alu_b_17_ #b1 #b0) (ite alu_b_16_ #b1 #b0) (ite alu_b_15_ #b1 #b0) (ite alu_b_14_ #b1 #b0) (ite alu_b_13_ #b1 #b0) (ite alu_b_12_ #b1 #b0) (ite alu_b_11_ #b1 #b0) (ite alu_b_10_ #b1 #b0) (ite alu_b_9_ #b1 #b0) (ite alu_b_8_ #b1 #b0) (ite alu_b_7_ #b1 #b0) (ite alu_b_6_ #b1 #b0) (ite alu_b_5_ #b1 #b0) (ite alu_b_4_ #b1 #b0) (ite alu_b_3_ #b1 #b0) (ite alu_b_2_ #b1 #b0) (ite alu_b_1_ #b1 #b0) (ite alu_b_0_ #b1 #b0))
      )
   )
)
(=>
    ( and
       ( = addsub_fn_0_ fresh_value_addsub_fn_0__v3 )
       ( = addsub_fn_1_ fresh_value_addsub_fn_1__v3 )
    )
    ( =
      (concat (ite result_31_ #b1 #b0) (ite result_30_ #b1 #b0) (ite result_29_ #b1 #b0) (ite result_28_ #b1 #b0) (ite result_27_ #b1 #b0) (ite result_26_ #b1 #b0) (ite result_25_ #b1 #b0) (ite result_24_ #b1 #b0) (ite result_23_ #b1 #b0) (ite result_22_ #b1 #b0) (ite result_21_ #b1 #b0) (ite result_20_ #b1 #b0) (ite result_19_ #b1 #b0) (ite result_18_ #b1 #b0) (ite result_17_ #b1 #b0) (ite result_16_ #b1 #b0) (ite result_15_ #b1 #b0) (ite result_14_ #b1 #b0) (ite result_13_ #b1 #b0) (ite result_12_ #b1 #b0) (ite result_11_ #b1 #b0) (ite result_10_ #b1 #b0) (ite result_9_ #b1 #b0) (ite result_8_ #b1 #b0) (ite result_7_ #b1 #b0) (ite result_6_ #b1 #b0) (ite result_5_ #b1 #b0) (ite result_4_ #b1 #b0) (ite result_3_ #b1 #b0) (ite result_2_ #b1 #b0) (ite result_1_ #b1 #b0) (ite result_0_ #b1 #b0))
      ( ite
        ( bvult
          (concat (ite alu_a_31_ #b1 #b0) (ite alu_a_30_ #b1 #b0) (ite alu_a_29_ #b1 #b0) (ite alu_a_28_ #b1 #b0) (ite alu_a_27_ #b1 #b0) (ite alu_a_26_ #b1 #b0) (ite alu_a_25_ #b1 #b0) (ite alu_a_24_ #b1 #b0) (ite alu_a_23_ #b1 #b0) (ite alu_a_22_ #b1 #b0) (ite alu_a_21_ #b1 #b0) (ite alu_a_20_ #b1 #b0) (ite alu_a_19_ #b1 #b0) (ite alu_a_18_ #b1 #b0) (ite alu_a_17_ #b1 #b0) (ite alu_a_16_ #b1 #b0) (ite alu_a_15_ #b1 #b0) (ite alu_a_14_ #b1 #b0) (ite alu_a_13_ #b1 #b0) (ite alu_a_12_ #b1 #b0) (ite alu_a_11_ #b1 #b0) (ite alu_a_10_ #b1 #b0) (ite alu_a_9_ #b1 #b0) (ite alu_a_8_ #b1 #b0) (ite alu_a_7_ #b1 #b0) (ite alu_a_6_ #b1 #b0) (ite alu_a_5_ #b1 #b0) (ite alu_a_4_ #b1 #b0) (ite alu_a_3_ #b1 #b0) (ite alu_a_2_ #b1 #b0) (ite alu_a_1_ #b1 #b0) (ite alu_a_0_ #b1 #b0))
          (concat (ite alu_b_31_ #b1 #b0) (ite alu_b_30_ #b1 #b0) (ite alu_b_29_ #b1 #b0) (ite alu_b_28_ #b1 #b0) (ite alu_b_27_ #b1 #b0) (ite alu_b_26_ #b1 #b0) (ite alu_b_25_ #b1 #b0) (ite alu_b_24_ #b1 #b0) (ite alu_b_23_ #b1 #b0) (ite alu_b_22_ #b1 #b0) (ite alu_b_21_ #b1 #b0) (ite alu_b_20_ #b1 #b0) (ite alu_b_19_ #b1 #b0) (ite alu_b_18_ #b1 #b0) (ite alu_b_17_ #b1 #b0) (ite alu_b_16_ #b1 #b0) (ite alu_b_15_ #b1 #b0) (ite alu_b_14_ #b1 #b0) (ite alu_b_13_ #b1 #b0) (ite alu_b_12_ #b1 #b0) (ite alu_b_11_ #b1 #b0) (ite alu_b_10_ #b1 #b0) (ite alu_b_9_ #b1 #b0) (ite alu_b_8_ #b1 #b0) (ite alu_b_7_ #b1 #b0) (ite alu_b_6_ #b1 #b0) (ite alu_b_5_ #b1 #b0) (ite alu_b_4_ #b1 #b0) (ite alu_b_3_ #b1 #b0) (ite alu_b_2_ #b1 #b0) (ite alu_b_1_ #b1 #b0) (ite alu_b_0_ #b1 #b0))
        )
        ( _ bv1 32 )
        ( _ bv0 32 )
      )
   )
)
)))))
(check-sat)
(exit)
