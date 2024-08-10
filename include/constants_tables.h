/*Adress method table according operations*/

static char* operation_mode[16][3] = {
	/*     Destination   Source          */
		 {"123","0123","mov"},
 {"0123","0123","cmp"},
 {"123","0123","add"},
		 {"123","0123","sub"},
 {"123","1","lea"},
 {"123"," ","clr"},
 {"123"," ","not"},
		 {"123"," ","inc"},
		 {"123"," ","dec"},
		 {"12"," ","jmp"},
		 {"12"," ","bne"},
		 {"123"," ","red"},
 {"0123"," ","prn"},
		 {"12"," ","jsr"},
		 {" "," ","rts"},
		 {" "," ","stop"}
};
