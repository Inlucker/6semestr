(defun fn(x)
	(cond (end_test end_value)
		  (add_test (add_fun add_value (fn changed1_x)))
		  (T (fn changed2_x))))
		  
(defun extract_symbols (lst)
	(cond ((null lst) NIL)
		  ((symbolp (car lst))
					(cons (car lst)
						  (extract_symbols (cdr lst))))))
						
(defun fn(x)
	(cond (end_test end_value)
		  (T (combiner (fn changed1_x)
					   (fn changed2_x)))))
					 
(defun first_number (lst)
	(cond ((numberp lst) lst)
		  ((atom lst) NIL)
		  (T (or (first_number (car lst))
				 (first_number (cdr lst))))))
				 
(defun cons_slls(lst)
	(if (atom lst) 0
		(+ (length lst)
		   (reduce #'+
				   (mapcar #'cons_slls lst)))))
				   
(defun into_one_level(lst rst)
	(cond ((null lst) rst)
		  ((atom lst) (cons lst rst))
		  (T (into_one_level (car lst) 
							 (into_one_level (cdr lst) rst)))))
							 
(boundp 'symbol)
(fboundp 'symbol)

(pafprop 'symbol property-name property-value)
(remprop 'symbol property-name)
(symbol-plist 'symbol)

&optional
(defun tt1 (x &optional y)(list x y))
(defun tt1 (x &optional (y (+ x 1)))(list x y))

&rest
(defun average (&rest arg)
	(/ (reduce #'+ arg)(length arg) 1.0))
(average 2 3 4 5)

(defun f1 (&key x y)(list x y))
(f1)
(f1 :x 1 :y 2)
