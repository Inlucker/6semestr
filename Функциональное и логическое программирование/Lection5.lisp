(defun fun(x)
	(cond (end_test end_value)
		  (T (fun changed_x))))
;	  
(a b c)
(((a b)) c)

(defun first-a(lst)
	(cond ((atom lst) lst)
		  (T (first-a (car lst)))))
; 
((a 2) (3 c) d)

(defun find-first-odd(lst)
	(cond ((null lst) NIL)
		  ((oddp (first-a lst)) (first-a lst))
		  (T (find-first-odd (cdr lst)))))
		  
(find-first-odd lst)
;
(defun my-nth(lst n)
	(cond ((null lst) nil)
		  ((= n 0) (car lst))
		  (T (my-nth (cdr lst) (- n 1)))))
;
(defun funс (x)
	(cond (end_test end_value)
		  (T (add_function add_value
						(func change_x)))))
;
(defun my-length(lst)
	(cond ((null lst) 0)
		  (T (+ 1 (my-length (cdr lst))))))
;
(defun into-one-level(lst)
	(cond ((null lst) nil)
		  ((atom lst) (cons lst NIL))
		  (T (append (into-one-level (car lst))
					 (into-one-level (cdr lst))))))

(setf lst '((a) (b c) (d e f)))
(into-one-level lst)
;
(defun insert-help(x lst)
	(cond ((null lst) (list x))
		  ((<= x (car lst)) (cons x lst))
		  (T (cons (car lst) (insert-help x (cdr lst))))))

(defun sort-help(lst1 lst2)
	(cond ((null lst1) lst2)
		  (T (sort-help (cdr lst1) (insert-help (car lst1) lst2)))))
		  
(defun sort-ins(lst)
	(sort-help lst ()))

(setf lst '(1 4 3 2 5))
(sort-ins lst)