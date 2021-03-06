(set-option :produce-interpolants true)
(declare-sort A 0)
(declare-fun x1 () A)
(declare-fun x2 () A)
(declare-fun f (A) A)

(assert (! (distinct (f x1) (f x2)) :named part_a))
(assert (! (= x1 x2) :named part_b))

(check-sat)
(get-interpolant part_a part_b)
