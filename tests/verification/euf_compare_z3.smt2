(set-option :produce-interpolants true)

(declare-sort A)
(declare-fun z1 () A)
(declare-fun z2 () A)
(declare-fun y1 () A)
(declare-fun y2 () A)
(declare-fun s1 () A)
(declare-fun s2 () A)
(declare-fun v () A)
(declare-fun t () A)
(declare-fun f (A A) A)

(push)

(assert (! (and 
(= (f z1 v) s1) 
(= (f z2 v) s2) 
(= (f (f y1 v) (f y2 v)) t)
) :named f1))

(assert (! (and 
(= z1 z2)
(= z2 y1)
(= z2 y2)
(distinct (f s2 s1) t)
) :named f2))

(check-sat)
(get-interpolant f1 f2)

(pop)

;; -------------------------------------


(push)

(assert (! (and 
(= (f z1 v) s1) 
(= (f z2 v) s2) 
(= (f (f y1 v) (f y2 v)) t)
) :named f1))

(assert (! (and 
(= z1 z2)
(= z1 y1)
(= z2 y2)
(distinct (f s1 s2) t)
) :named f2))

(check-sat)
(get-interpolant f1 f2)

(pop)

;; -------------------------------------

(push)

(define-fun interpolant_1 () Bool
 (let ((a!1 (and (not (= s1 s2)) (or (= s2 s1) (not (= z2 z1))))))
  (or (= (f s1 s2) t) a!1 (not (= z2 z1)) (not (= y1 z1)) (not (= y2 z1)))))

(define-fun interpolant_2 () Bool
 (let ((a!1 (and (not (= s1 s2)) (or (= s2 s1) (not (= z2 z1))))))
  (or (= (f s2 s1) t) (not (= z2 z1)) a!1 (not (= y1 z1)) (not (= y2 z1)))))

;(define-fun uniform_inter () Bool
;(and 
;(=> (and (= z1 y1) (= z1 y2)) (= (f s1 s1) t))
;(=> (and (= z1 y1) (= z2 y2)) (= (f s1 s2) t))
;(=> (and (= z2 y1) (= z1 y2)) (= (f s2 s1) t))
;(=> (and (= z2 y1) (= z2 y2)) (= (f s2 s2) t))
;))

(define-fun uniform_inter () Bool
(and 
(=> (and (= z1 y1) (= z1 y2)) (= (f s1 s1) t))
(=> (and (= z1 y1) (= z1 y2) (= z2 y1)) (= (f s1 s2) t))
(=> (and (= z2 y1) (= z1 y2)) (= (f s2 s1) t))
(=> (and (= z2 y1) (= z1 y2) (= z1 y1)) (= (f s2 s2) t))
))

(push)
(assert (not (=> uniform_inter interpolant_1)))
(echo "--Uniform interpolant implies interpolant_1")
(check-sat)
(pop)

(push)
(assert (not (=> interpolant_1 uniform_inter)))
(echo "--interpolant_1 doesnt imply Uniform interpolant")
(check-sat)
(pop)

(push)
(assert (not (=> uniform_inter interpolant_2)))
(echo "--Uniform interpolant implies interpolant_2")
(check-sat)
(pop)

(push)
(assert (not (=> interpolant_2 uniform_inter)))
(echo "--interpolant_2 doesnt imply Uniform interpolant")
(check-sat)
(pop)

(pop)
