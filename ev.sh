set -- $( cat $1 |
	head -n 2 |
	awk '{print $1}'
	)
