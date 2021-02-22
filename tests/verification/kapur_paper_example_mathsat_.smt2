(set-option :produce-interpolants true)
(declare-sort A 0)
(declare-fun y1 () A)
(declare-fun y2 () A)
(declare-fun z1 () A)
(declare-fun z2 () A)
(declare-fun s1 () A)
(declare-fun s2 () A)
(declare-fun t () A)
(declare-fun v () A)
(declare-fun f (A A) A)

(assert (! (and
(= (f z1 v) s1)
(= (f z2 v) s2)
(= (f (f y1 v) (f y2 v)) t)
) :interpolation-group part_a))

(assert (! (and 
(= z1 y1)
(= z1 y2)
(distinct (f s1 s1) t)
) :interpolation-group part_b))

(check-sat)
(get-interpolant (part_a))
(exit)
