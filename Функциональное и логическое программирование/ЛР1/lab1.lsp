;Задание 2
;1) второй
(car (cdr '(1 2)))
;2) третий
(car (cdr (cdr '(1 2 3))))
;3) четвёртый
(car (cdr (cdr (cdr '(1 2 3 4)))))

;Задание 3
;a) (caadr '((blue cube) (red pyramid)))
RED
;b) (cdar '((abc) (def) (ghi)))
() ;NIL
;c) (cadr '((abc) (def) (ghi)))
(DEF)
;d) (caddr '((abc) (def) (ghi)))
(GHI)

;Задание 4
;(list 'Fred 'and 'Wilma)	
(FRED AND WILMA)
;(list 'Fred '(and Wilma))
(FRED (AND WILMA))
;(cons nil nil)
(NIL) ;(())
;(cons T nil)
(T)
;(cons nil T)
(NIL.T)
;(list nil)
(NIL) ;(())
;(cons '(T) nil)
((T))
;(list '(one two) '(free temp))
((ONE TWO)(FREE TEMP))
;(cons 'Fred '(and Wilma))
(FRED AND WILMA)
;(cons 'Fred '(Wilma))
(FRED WILMA)
;(list nil nil)
(NIL NIL) ;(() ())
;(list T nil)
(T NIL) ;(T ())
;(list nil T)
(NIL T) ;(() T)
;(cons T (list nil))
(T NIL)
;(list '(T) nil)
((T) NIL)
;(cons '(one two) '(free temp))
((ONE TWO) FREE TEMP)

;Задание 5
;1)
(defun f1 (ar1 ar2 ar3 ar4)
	(list (list ar1 ar2)(list ar3 ar4)))
;2)
(defun f2 (ar1 ar2)
	(list (list ar1)(list ar2)))
;3)
(defun f3 (ar1)
	(list (list (list ar1))))
;cons 1)
(defun f1 (ar1 ar2 ar3 ar4)
	(cons
		(cons
			ar1
			(cons ar2 nil))
		(cons
			(cons
				ar3
				(cons ar4 nil))
			nil)))
;cons 2)
(defun f2 (ar1 ar2)
	(cons
		(cons ar1 nil)
		(cons
			(cons ar2 nil)
			nil)))
;cons 3)
(defun f3 (ar1)
	(cons
		(cons
			(cons ar1 nil)
			nil)
		nil))