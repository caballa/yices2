(set-logic QF_LIA)
(declare-fun x () Int)
(assert (and (= (div x (- 4)) 2) (not (= x (- 8)))))
(check-sat)
(get-model)
(get-value (x (div x (- 4)) (mod x (- 4))))