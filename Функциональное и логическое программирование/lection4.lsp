;1
(defun consist-of (lst)
	(if (member (car lst)(cdr lst)) 1 0))
	
(defun all-last-element (lst)
	(if (eql (consist-of lst) 0)(list (car lst))()))

(defun collection-to-set (lst)
	(mapcon #'all-last-element lst))
	
(collection-to-set '(i t i g t k s i f k))

;2
(defun decart (lstX lstY)
	(mapcan #'(lambda (x)
					(mapcar #'(lambda (y)
								(list x y)) lstY))
											lstX))
												
(decart '(a b) '(1 2)) = ((a 1)(a 2)(b 1)(b 2))

;рекурсия1
(defun my-member (el lst)
	(cond ((null lst) NIL)
		  ((equal el (car lst)) T)
		  (t (my-member el (cdr lst)))))
		  
(my-member 'a '(b a c)) = T
(my-member nil ()) = nil

;поменяли проверки местами
(defun my-member (el lst)
	(cond ((equal el (car lst)) T)
		  ((null lst) NIL)
		  (t (my-member el (cdr lst)))))
		  
(my-member nil ()) = T


;рекурсия2
(defun my-reverse (lst)
	(cond ((null lst) lst)
		  (T (append (my-reverse (cdr lst))
						(cons (car lst) lst)))))

;еще один способ
(defun my-reverse1 (lst)
	...)
	
(defun move-to (lst result)
	(cond ((null lst) result)
		  (T (move-to (cdr lst) (cons (car lst) result)))))
		  
(defun my-reverse1 (lst)
	(move-to lst ()))