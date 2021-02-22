(set-option :produce-interpolants true)
(declare-sort A 0)
(declare-fun a () A)
(declare-fun b () A)
(declare-fun c () A)
(declare-fun f (A) A)
(declare-fun g (A) A)
(declare-fun h (A A) A)

(assert (!
(and
(= (f a) a)
(= (h (f (f a)) (f a)) (h c c))
)
:named part_a))

(assert (!
(and 
(= a b)
(= (g b) b)
(distinct (h (g a) (g (g b))) (h c c))
)
:named part_b))

(check-sat)
(get-interpolant part_a part_b)
